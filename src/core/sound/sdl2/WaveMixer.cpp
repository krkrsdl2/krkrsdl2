/* SPDX-License-Identifier: MIT */
/* Copyright (c) Kirikiri SDL2 Developers */

#include "tjsCommHead.h"
#include "WaveMixer.h"
#include "WaveImpl.h"
#include "DebugIntf.h"
#include "SysInitIntf.h"
#include <unordered_set>

#ifndef TVP_FAUDIO_IMPLEMENT
class tTVPAudioRenderer;
static tTVPAudioRenderer *TVPAudioRenderer;

#ifdef __APPLE__
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#else
#include <AL/al.h>
#include <AL/alc.h>
#endif
#ifdef __EMSCRIPTEN__
#define AL_FORMAT_MONO_FLOAT32   0x10010
#define AL_FORMAT_STEREO_FLOAT32 0x10011
#endif

class tTVPSoundBuffer : public iTVPSoundBuffer
{
	ALuint _alSource;
	ALenum _alFormat;
	ALuint *_bufferIds;
	ALuint *_bufferIds2;
	tjs_uint *_bufferSize;
	tjs_uint _bufferCount;
	int _bufferIdx = -1;
	tTVPWaveFormat _format;
	tTJSCriticalSection _buffer_mtx;
	tjs_uint _sentSamples = 0;
	bool _playing = false;
	int _frame_size = 0;
	bool has_format = false;
public:
	tTVPSoundBuffer(tTVPWaveFormat &desired, int bufcount)
	{
		_bufferCount = bufcount;
		_frame_size = desired.BytesPerSample * desired.Channels;
		_bufferIds = new ALuint[bufcount];
		_bufferIds2 = new ALuint[bufcount];
		_bufferSize = new tjs_uint[bufcount];
		_format = desired;
		alGenSources(1, &_alSource);
		alGenBuffers(_bufferCount, _bufferIds);
		alSourcef(_alSource, AL_GAIN, 1.0f);
		has_format = true;
		switch (desired.Channels)
		{
			case 1:
			{
				switch (desired.BitsPerSample)
				{
					case 8:
					{
						_alFormat = AL_FORMAT_MONO8;
						break;
					}
					case 16:
					{
						_alFormat = AL_FORMAT_MONO16;
						break;
					}
					case 32:
					{
						_alFormat = AL_FORMAT_MONO_FLOAT32;
						break;
					}
					default:
					{
						has_format = false;
						break;
					}
				}
				break;
			}
			case 2:
			{
				switch (desired.BitsPerSample)
				{
					case 8:
					{
						_alFormat = AL_FORMAT_STEREO8;
						break;
					}
					case 16:
					{
						_alFormat = AL_FORMAT_STEREO16;
						break;
					}
					case 32:
					{
						_alFormat = AL_FORMAT_STEREO_FLOAT32;
						break;
					}
					default:
					{
						has_format = false;
						break;
					}
				}
				break;
			}
			default:
			{
				has_format = false;
				break;
			}
		}
	}

	virtual ~tTVPSoundBuffer()
	{
		alDeleteBuffers(_bufferCount, _bufferIds);
		alDeleteSources(1, &_alSource);
		delete[] _bufferIds;
		delete[] _bufferIds2;
		delete[] _bufferSize;
	}

	virtual void Release() override
	{
		delete this;
	}

	bool IsBufferValid() override
	{
		ALint processed = 0;
		alGetSourcei(_alSource, AL_BUFFERS_PROCESSED, &processed);
		if (processed > 0)
		{
			return true;
		}
		ALint queued = 0;
		alGetSourcei(_alSource, AL_BUFFERS_QUEUED, &queued);
		return queued < _bufferCount;
	}

	virtual void AppendBuffer(const void *buf, unsigned int len) override
	{
		if (len <= 0)
		{
			return;
		}
		if (!has_format)
		{
			return;
		}
		tTJSCriticalSectionHolder holder(_buffer_mtx);

		ALint processed = 0;
		alGetSourcei(_alSource, AL_BUFFERS_PROCESSED, &processed);
		if (processed > 0)
		{
			alSourceUnqueueBuffers(_alSource, processed, _bufferIds2);
			CheckALError("alSourceUnqueueBuffers");
			for (int i = 0; i < processed; i += 1)
			{
				for (int j = 0; j < _bufferCount; j += 1)
				{
					if (_bufferIds[j] == _bufferIds2[i])
					{
						_sentSamples += _bufferSize[j] / _frame_size;
						break;
					}
				}
			}
		}

		ALint queued = 0;
		alGetSourcei(_alSource, AL_BUFFERS_QUEUED, &queued);

		if (queued >= _bufferCount)
		{
			return;
		}
		_bufferIdx += 1;
		if (_bufferIdx >= _bufferCount)
		{
			_bufferIdx = 0;
		}
		ALuint bufid = _bufferIds[_bufferIdx];
		alBufferData(bufid, _alFormat, buf, len, _format.SamplesPerSec);
		CheckALError("alBufferData");
		alSourceQueueBuffers(_alSource, 1, &bufid);
		CheckALError("alSourceQueueBuffers");
		EnsurePlayState();
	}

	void EnsurePlayState()
	{
		ALenum state;
		alGetSourcei(_alSource, AL_SOURCE_STATE, &state);
		CheckALError("alGetSourcei AL_SOURCE_STATE");
		if (_playing)
		{
			if (state != AL_PLAYING)
			{
				alSourcePlay(_alSource);
				CheckALError("alSourcePlay");
			}
		}
		else
		{
			if (state == AL_PLAYING)
			{
				alSourcePause(_alSource);
				CheckALError("alSourcePause");
			}
		}
	}

	void Reset() override
	{
		_sentSamples = 0;
		tTJSCriticalSectionHolder holder(_buffer_mtx);
		alSourceRewind(_alSource);
		alSourcei(_alSource, AL_BUFFER, 0);
	}

	void Pause() override
	{
		_playing = false;
		EnsurePlayState();
	}

	void Play() override
	{
		_playing = true;
		EnsurePlayState();
	}

	void Stop() override
	{
		_playing = false;
		EnsurePlayState();
		alSourceStop(_alSource);
		CheckALError("alSourceStop");
		Reset();
		_bufferIdx = -1;
	}

	void SetVolume(float volume) override
	{
		alSourcef(_alSource, AL_GAIN, volume);
		CheckALError("alSourcef AL_GAIN");
	}

	float GetVolume() override
	{
		float volume = 0;
		alGetSourcef(_alSource, AL_GAIN, &volume);
		return volume;
	}

	void SetPan(float pan) override
	{
		float sourcePosAL[] = { pan, 0.0f, 0.0f };
		alSourcefv(_alSource, AL_POSITION, sourcePosAL);
	}

	float GetPan() override
	{
		float sourcePosAL[3];
		alGetSourcefv(_alSource, AL_POSITION, sourcePosAL);
		return sourcePosAL[0];
	}

	bool IsPlaying() override
	{
		ALenum state;
		alGetSourcei(_alSource, AL_SOURCE_STATE, &state);
		return state == AL_PLAYING;
	}

	void SetPosition(float x, float y, float z) override
	{
		float sourcePosAL[] = { x, y, z };
		alSourcefv(_alSource, AL_POSITION, sourcePosAL);
		CheckALError("alSourcefv AL_POSITION");
	}

	int GetRemainBuffers() override
	{
		ALint processed = 0;
		ALint queued = 0;
		alGetSourcei(_alSource, AL_BUFFERS_PROCESSED, &processed);
		alGetSourcei(_alSource, AL_BUFFERS_QUEUED, &queued);
		return queued - processed;
	}

	tjs_uint GetLatencySamples() override
	{
		tTJSCriticalSectionHolder holder(_buffer_mtx);
		ALint offset = 0, queued = 0;
		alGetSourcei(_alSource, AL_BYTE_OFFSET, &offset);
		alGetSourcei(_alSource, AL_BUFFERS_QUEUED, &queued);
		int remainBuffers = queued;
		if (remainBuffers == 0)
		{
			return 0;
		}
		tjs_int total = -offset;
		for (int i = 0; i < remainBuffers; i += 1)
		{
			int idx = _bufferIdx + 1 - remainBuffers + i;
			if (idx >= _bufferCount)
			{
				idx -= _bufferCount;
			}
			else if (idx < 0)
			{
				idx += _bufferCount;
			}
			total += _bufferSize[idx];
		}
		return total / _frame_size;
	}

	virtual tjs_uint GetCurrentPlaySamples() override
	{
		ALint offset = 0;
		alGetSourcei(_alSource, AL_SAMPLE_OFFSET, &offset);
		return _sentSamples + offset;
	}

	static void CheckALError(const char *funcname);
};

class tTVPAudioRenderer
{
	ALCdevice *_device = nullptr;
	ALCcontext *_context = nullptr;
	std::unordered_set<tTVPSoundBuffer*> _streams;
public:
	virtual ~tTVPAudioRenderer()
	{
		if (_context)
		{
			alcMakeContextCurrent(nullptr);
			alcDestroyContext(_context);
		}
		if (_device)
		{
			alcCloseDevice(_device);
		}
	}
	bool Init()
	{
		_device = alcOpenDevice(nullptr);
		if (!_device)
		{
			return false;
		}

		_context = alcCreateContext(_device, nullptr);
		alcMakeContextCurrent(_context);

		return true;
	}

	virtual tTVPSoundBuffer* CreateStream(tTVPWaveFormat &fmt, int bufcount)
	{
		tTVPSoundBuffer* s = new tTVPSoundBuffer(fmt, bufcount);
		_streams.emplace(s);
		return s;
	}

	ALCcontext *GetContext()
	{
		return _context;
	}
};

void tTVPSoundBuffer::CheckALError(const char *funcname)
{
	ALCcontext *ctx = ((tTVPAudioRenderer*)TVPAudioRenderer)->GetContext();
	if (alcGetCurrentContext() != ctx)
	{
		alcMakeContextCurrent(ctx);
	}
	ALenum err = alGetError();
	if (AL_NO_ERROR == err)
	{
		return;
	}
	TVPAddImportantLog(ttstr(funcname) + ttstr(": OpenAL Error ") + TJSInt32ToHex(err, 0));
}
#endif

#ifndef TVP_FAUDIO_IMPLEMENT
static tTVPAudioRenderer *CreateAudioRenderer()
{
	tTVPAudioRenderer *renderer = nullptr;
	renderer = new tTVPAudioRenderer;
	renderer->Init();
	return renderer;
}
#endif

void TVPInitDirectSound()
{
#ifndef TVP_FAUDIO_IMPLEMENT
	if (!TVPAudioRenderer)
	{
		TVPAudioRenderer = CreateAudioRenderer();
	}
#endif
}

void TVPUninitDirectSound()
{
}

iTVPSoundBuffer* TVPCreateSoundBuffer(tTVPWaveFormat &fmt, int bufcount)
{
	iTVPSoundBuffer* stream = nullptr;
#ifndef TVP_FAUDIO_IMPLEMENT
	stream = TVPAudioRenderer->CreateStream(fmt, bufcount);
#endif
	return stream;
}
