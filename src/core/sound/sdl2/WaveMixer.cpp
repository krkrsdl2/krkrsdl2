#include "tjsCommHead.h"
#include "WaveMixer.h"
#include "WaveImpl.h"
#include "DebugIntf.h"
#include <SDL.h>
#include <unordered_set>

class iTVPAudioRenderer;
static iTVPAudioRenderer *TVPAudioRenderer;

template<int ch>
void MixAudioS16CPP(void *dst, const void *src, int samples, int16_t *volume) {
	int16_t *dst16 = (int16_t *)dst;
	const int16_t *src16 = (const int16_t *)src;
	while (samples--) {
		for (int i = 0; i < ch; ++i) {
			int src_sample = *src16++;
			src_sample = (src_sample * volume[i]) >> 14;
			int dest_sample = *dst16;
			if (src_sample > 0 && dest_sample > 0) {
				dest_sample = src_sample + dest_sample - ((dest_sample * src_sample + 0x8000) >> 15);
			} else if (src_sample < 0 && dest_sample < 0) {
				dest_sample = src_sample + dest_sample + ((dest_sample * src_sample) >> 15);
			} else {
				dest_sample += src_sample;
			}
			*dst16++ = dest_sample;
		}
	}
}

template<int ch>
void MixAudioF32CPP(void *dst, const void *src, int samples, int16_t *volume) {
	float *dst32 = (float *)dst;
	const float *src32 = (const float *)src;
	const float fmaxvolume = 1.0f / 16384/*tTVPSoundBuffer::MAX_VOLUME*/;
	float fvolume[ch];
	for (int i = 0; i < ch; ++i)
		fvolume[i] = volume[i] * fmaxvolume;
	while (samples--) {
		for (int i = 0; i < ch; ++i) {
			float src_sample = SDL_SwapFloatLE(*src32++) * fvolume[i];
			float dest_sample = SDL_SwapFloatLE(*dst32);
			if (src_sample > 0 && dest_sample > 0) {
				dest_sample = src_sample + dest_sample - dest_sample * src_sample;
			} else if (src_sample < 0 && dest_sample < 0) {
				dest_sample = src_sample + dest_sample + dest_sample * src_sample;
			} else {
				dest_sample += src_sample;
			}
			*(dst32++) = SDL_SwapFloatLE(dest_sample);
		}
	}
}

typedef void(FAudioMix)(void *dst, const void *src, int samples, int16_t *volume);
static FAudioMix *_AudioMixS16[8] = { // 7.1 max
	&MixAudioS16CPP<1>,
	&MixAudioS16CPP<2>,
	&MixAudioS16CPP<3>,
	&MixAudioS16CPP<4>,
	&MixAudioS16CPP<5>,
	&MixAudioS16CPP<6>,
	&MixAudioS16CPP<7>,
	&MixAudioS16CPP<8>
};
static FAudioMix *_AudioMixF32[8] = { // 7.1 max
	&MixAudioF32CPP<1>,
	&MixAudioF32CPP<2>,
	&MixAudioF32CPP<3>,
	&MixAudioF32CPP<4>,
	&MixAudioF32CPP<5>,
	&MixAudioF32CPP<6>,
	&MixAudioF32CPP<7>,
	&MixAudioF32CPP<8>
};

class tTVPSoundBuffer : public iTVPSoundBuffer {
public:
	bool _playing = false;
	float _volume = 1;
	float _pan = 0;
	const signed int MAX_VOLUME = 16384; // limit in signed 16bit
	int16_t _volume_raw[8];
	SDL_AudioCVT *_cvt = nullptr;
	std::vector<uint8_t> _cvtbuf;
	int _frame_size = 0;

	void RecalcVolume() {
		if (_pan > 0) {
			_volume_raw[0] = (1.0f - _pan) * _volume * MAX_VOLUME;
		} else {
			_volume_raw[0] = _volume * MAX_VOLUME;
		}
		if (_pan < 0) {
			_volume_raw[1] = (_pan + 1.0f) * _volume * MAX_VOLUME;
		} else {
			_volume_raw[1] = _volume * MAX_VOLUME;
		}
		_volume_raw[2] = _volume_raw[0]; // for SIMD
		_volume_raw[3] = _volume_raw[1];
	}
	SDL_mutex *_buffer_mtx;
	std::deque<std::vector<uint8_t> > _buffers;
	tjs_uint _sendedFrontBuffer = 0;
	tjs_uint _sendedSamples = 0, _inCachedSamples = 0;

	tTVPSoundBuffer(int framesize, SDL_AudioCVT *cvt) : _frame_size(framesize), _cvt(cvt) {
		_buffer_mtx = SDL_CreateMutex();
		RecalcVolume();
		if (cvt) {
			_cvtbuf.resize(/*2352*/ 2400 * 2 * 4 * _cvt->len_mult); // IEEE f.32 stereo 48000kHz
			_cvt->buf = &_cvtbuf.front();
		}
	}
	virtual ~tTVPSoundBuffer();
	virtual void Release() override { delete this; }
	virtual void Play() override {
		_playing = true;
	}
	virtual void Pause() override {
		_playing = false;
	}
	virtual void Stop() override {
		_playing = false;
		Reset();
	}
	virtual void Reset() override {
		SDL_LockMutex(_buffer_mtx);
		_buffers.clear();
		_inCachedSamples = 0;
		_sendedFrontBuffer = 0;
		_sendedSamples = 0;
		SDL_UnlockMutex(_buffer_mtx);
	}
	virtual bool IsPlaying() override {
		return _playing;
	}
	virtual void SetVolume(float v) override {
		_volume = v;
		RecalcVolume();
	}
	virtual float GetVolume() override { return _volume; }
	virtual void SetPan(float v) override {
		_pan = v;
		RecalcVolume();
	}
	virtual float GetPan() override { return _pan; }
	virtual void AppendBuffer(const void *_inbuf, unsigned int inlen/*, int tag = 0*/) override {
		if (_cvt) {
			std::vector<uint8_t> buffer;
			uint8_t* inbuf = (uint8_t*)_inbuf;
			int buflen = _frame_size * 2352;
			_cvt->len = buflen;
			while (inlen > buflen) { // fill 2352 samples to fit 48k/44.1k
				memcpy(_cvt->buf, inbuf, buflen);
				SDL_ConvertAudio(_cvt);
				buffer.insert(buffer.end(), _cvt->buf, _cvt->buf + _cvt->len_cvt);
				inlen -= buflen;
				inbuf += buflen;
			}
			if (inlen > 0) {
				int buflen = inlen;
				memcpy(_cvt->buf, inbuf, buflen);
				_cvt->len = buflen;
				SDL_ConvertAudio(_cvt);
				buffer.insert(buffer.end(), _cvt->buf, _cvt->buf + _cvt->len_cvt);
			}
			SDL_LockMutex(_buffer_mtx);
			_inCachedSamples += buffer.size() / _frame_size;
			_buffers.emplace_back();
			_buffers.back().swap(buffer);
			SDL_UnlockMutex(_buffer_mtx);
		} else {
			SDL_LockMutex(_buffer_mtx);
			_buffers.emplace_back((uint8_t*)_inbuf, ((uint8_t*)_inbuf) + inlen);
			_inCachedSamples += inlen / _frame_size;
			SDL_UnlockMutex(_buffer_mtx);
		}
	}
	virtual bool IsBufferValid() override {
		return true;
	}
	virtual tjs_uint GetLatencySamples() override;
	virtual int GetRemainBuffers() override {
		return _buffers.size();
	}
	virtual tjs_uint GetCurrentPlaySamples() override;
	virtual float GetLatencySeconds() override;

	void FillBuffer(uint8_t *out, int len);
};

class iTVPAudioRenderer {
protected:
	SDL_AudioSpec _spec;
	SDL_mutex *_streams_mtx;
	std::unordered_set<tTVPSoundBuffer*> _streams;
	int _frame_size = 0;

public:
	iTVPAudioRenderer() {
		_streams_mtx = SDL_CreateMutex();
		memset(&_spec, 0, sizeof(_spec));
		_spec.freq = 48000;
		_spec.format = AUDIO_S16;
		_spec.channels = 2;
		_spec.callback = [](void *p, Uint8 *s, int l) {
			memset(s, 0, l);
			((iTVPAudioRenderer*)p)->FillBuffer(s, l);
		};
		_spec.userdata = this;
		_spec.size = 4;
		_frame_size = 4;
	}
	virtual ~iTVPAudioRenderer() {
		SDL_DestroyMutex(_streams_mtx);
	}
	void InitMixer() {
		if (SDL_Init(SDL_INIT_AUDIO) < 0) { // for format converter
			TVPAddLog(ttstr("Failed to initialize SDL audio: ") + ttstr(SDL_GetError()));
			return;
		}
	}

	FAudioMix *DoMixAudio;
	void SetupMixer() {
		if (_spec.format == AUDIO_S16LSB) {
			DoMixAudio = _AudioMixS16[_spec.channels - 1];
		} else if (_spec.format == AUDIO_F32LSB) {
			DoMixAudio = _AudioMixF32[_spec.channels - 1];
		} else {
			DoMixAudio = [](void *dst, const void *src, int samples, int16_t *volume) {};
		}
	}

	virtual bool Init() = 0;

	virtual tTVPSoundBuffer* CreateStream(tTVPWaveFormat &fmt, int bufcount) {
		SDL_AudioSpec spec;
		memset(&spec, 0, sizeof(spec));
		spec.freq = fmt.SamplesPerSec;
		spec.channels = fmt.Channels;
		if (fmt.IsFloat) {
			spec.format = AUDIO_F32LSB;
		} else {
			switch (fmt.BitsPerSample) {
			case 8: spec.format = AUDIO_S8; break;
			case 16: spec.format = AUDIO_S16LSB; break;
			case 32: spec.format = AUDIO_S32LSB; break;
			default: return nullptr;
			}
		}
		SDL_AudioCVT *cvt = nullptr;
		if (spec.freq != _spec.freq ||
			spec.channels != _spec.channels ||
			spec.format != _spec.format) {
			cvt = new SDL_AudioCVT;
			int err = SDL_BuildAudioCVT(cvt,
				spec.format, spec.channels, spec.freq,
				_spec.format, _spec.channels, _spec.freq);
			if (err != 1) {
				delete cvt; return nullptr;
			}
		}

		tTVPSoundBuffer* s = new tTVPSoundBuffer(fmt.BytesPerSample * fmt.Channels, cvt);
		SDL_LockMutex(_streams_mtx);
		_streams.emplace(s);
		SDL_UnlockMutex(_streams_mtx);
		return s;
	}

	void ReleaseStream(tTVPSoundBuffer* s) {
		SDL_LockMutex(_streams_mtx);
		_streams.erase(s);
		SDL_UnlockMutex(_streams_mtx);
	}

	void FillBuffer(Uint8 *buf, int len) {
		SDL_LockMutex(_streams_mtx);
		for (tTVPSoundBuffer* s : _streams) {
			s->FillBuffer(buf, len);
		}
		SDL_UnlockMutex(_streams_mtx);
	}

	int MixAudio(uint8_t *dst, uint8_t *src, int len, int16_t *vol) {
		int samples = len / _frame_size;
		DoMixAudio(dst, src, samples, vol);
		return samples;
	}

	const SDL_AudioSpec& GetSpec() {
		return _spec;
	}

	virtual int32_t GetUnprocessedSamples() { return 0; }
};

tTVPSoundBuffer::~tTVPSoundBuffer()
{
	Stop();
	TVPAudioRenderer->ReleaseStream(this);
	SDL_DestroyMutex(_buffer_mtx);
	if (_cvt) delete _cvt;
}

tjs_uint tTVPSoundBuffer::GetLatencySamples()
{
	int32_t samples = TVPAudioRenderer->GetUnprocessedSamples();
	return samples + _inCachedSamples;
}

tjs_uint tTVPSoundBuffer::GetCurrentPlaySamples()
{
	int32_t samples = TVPAudioRenderer->GetUnprocessedSamples();
	if (samples > _sendedSamples) return 0;
	return _sendedSamples - samples; // -GetLatencySamples();
}

float tTVPSoundBuffer::GetLatencySeconds()
{
	return GetLatencySamples() / TVPAudioRenderer->GetSpec().freq;
}

void tTVPSoundBuffer::FillBuffer(uint8_t *out, int len)
{
	if (!_playing) return;
	SDL_LockMutex(_buffer_mtx);
	while (len > 0 && !_buffers.empty()) {
		std::vector<uint8_t> &buf = _buffers.front();
		if (buf.size() > _sendedFrontBuffer) {
			int n = std::min((size_t)len, buf.size() - _sendedFrontBuffer);
			int samples = TVPAudioRenderer->MixAudio(out, &buf.front() + _sendedFrontBuffer, n, _volume_raw);
			_sendedSamples += samples;
			_inCachedSamples -= samples;
			_sendedFrontBuffer += n;
			out += n;
			len -= n;
		}
		if (_sendedFrontBuffer >= buf.size()) {
			_sendedFrontBuffer = 0;
			_buffers.pop_front();
		}
	}
	SDL_UnlockMutex(_buffer_mtx);
}

class tTVPAudioRendererSDL : public iTVPAudioRenderer {
	SDL_AudioDeviceID _playback_id;

public:
	bool Init() override {
		InitMixer();
		_playback_id = SDL_OpenAudioDevice(nullptr, false, &_spec, &_spec, SDL_AUDIO_ALLOW_ANY_CHANGE);
		if (_playback_id <= 0) {
			TVPAddLog(ttstr("Failed to open audio at ") + ttstr(_spec.freq) + ttstr("Hz: ") + ttstr(SDL_GetError()));
			return false;
		}
		_frame_size = SDL_AUDIO_BITSIZE(_spec.format) / 8 * _spec.channels;
		TVPAddLog(ttstr("Audio Device: ") + ttstr(SDL_GetCurrentAudioDriver()));
		SDL_PauseAudioDevice(_playback_id, false);
		SetupMixer();
		return true;
	}
};

static iTVPAudioRenderer *CreateAudioRenderer() {
	iTVPAudioRenderer *renderer = nullptr;
	renderer = new tTVPAudioRendererSDL;
	renderer->Init();
	return renderer;
}

void TVPInitDirectSound(int freq)
{
	if (!TVPAudioRenderer) {
		TVPAudioRenderer = CreateAudioRenderer();
	}
}

void TVPUninitDirectSound()
{
}

iTVPSoundBuffer* TVPCreateSoundBuffer(tTVPWaveFormat &fmt, int bufcount)
{
	return TVPAudioRenderer->CreateStream(fmt, bufcount);
}
