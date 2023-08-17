/* SPDX-License-Identifier: MIT */
/* Copyright (c) Kirikiri SDL2 Developers */

#include "tjsCommHead.h"

#include <string.h>
#include <assert.h>
#include <vector>
#include <algorithm>
#include "AudioDevice.h"
#include "MsgIntf.h"
#include "DebugIntf.h"

#ifdef TVP_FAUDIO_IMPLEMENT
#include <FAudio.h>

class FAudioStream;
class FAudioDevice : public iTVPAudioDevice
{
	FAudio* FAudioObj;
	FAudioMasteringVoice* MasteringVoiceObj;
	tjs_int Volume;

	std::vector<FAudioStream*> Children;

	float CalcVolume() const
	{
		float level = (float)Volume / 100000.0f;
		if (level < 0.0f)
		{
			level = 0.0f;
		}
		if (level > 1.0f)
		{
			level = 1.0f;
		}
		return level;
	}
public:
	FAudioDevice()
	{
		FAudioObj = nullptr;
		MasteringVoiceObj = nullptr;
		Volume = 100000;
	}

	virtual ~FAudioDevice() override
	{
	}

	virtual void Initialize(tTVPAudioInitParam& param) override
	{
		if (param.SampleRate < FAUDIO_MIN_SAMPLE_RATE || param.SampleRate > FAUDIO_MAX_SAMPLE_RATE)
		{
			TVPThrowExceptionMessage(TJS_W("Sample rate parameter is out of range"));
		}
		if (param.Channels > FAUDIO_MAX_AUDIO_CHANNELS)
		{
			TVPThrowExceptionMessage(TJS_W("Channel parameter is out of range"));
		}
		TVPAddLog(TJS_W("FAudio initializing..."));

		uint32_t flags = 0;
		uint32_t hr = FAudioCreate(&FAudioObj, flags, FAUDIO_DEFAULT_PROCESSOR);
		if (hr != 0)
		{
			TVPThrowExceptionMessage(TJS_W("Failed to call FAudioCreate"));
		}
		hr = FAudio_CreateMasteringVoice(
			FAudioObj,
			&MasteringVoiceObj,
			param.Channels,
			param.SampleRate,
			0,
			0,
			nullptr
		);
		if (hr != 0)
		{
			TVPThrowExceptionMessage(TJS_W("Failed to call FAudio_CreateMasteringVoice"));
		}

		FAudioVoiceDetails details;
		FAudioVoice_GetVoiceDetails(MasteringVoiceObj, &details);
		TVPAddLog(ttstr(TJS_W("Number of Channels: ")) + ttstr(to_tjs_string(details.InputChannels)));
		TVPAddLog(ttstr(TJS_W("Sample Rate: ")) + ttstr(to_tjs_string(details.InputSampleRate)));
	}

	virtual void Uninitialize() override
	{
		while (Children.size() != 0)
		{
			auto i = Children.begin();
			delete *i;
		}

		if (MasteringVoiceObj != nullptr)
		{
			FAudioVoice_DestroyVoice(MasteringVoiceObj);
			MasteringVoiceObj = nullptr;
		}
		if (FAudioObj != nullptr)
		{
			FAudio_Release(FAudioObj);
			FAudioObj = nullptr;
		}
	}

	virtual iTVPAudioStream* CreateAudioStream(tTVPAudioStreamParam& param) override;

	virtual void SetMasterVolume(tjs_int vol) override
	{
		if (Volume == vol)
		{
			return;
		}
		if (MasteringVoiceObj == nullptr)
		{
			return;
		}
		Volume = vol;
		FAudioVoice_SetVolume(MasteringVoiceObj, CalcVolume(), FAUDIO_COMMIT_NOW);
	}
	virtual tjs_int GetMasterVolume() const override
	{
		return Volume;
	}

	void AddStream(FAudioStream* stream)
	{
		Children.push_back(stream);
	}

	void DelStream(FAudioStream* stream)
	{
		auto i = std::remove(Children.begin(), Children.end(), stream);
		Children.erase(i, Children.end());
	}

	FAudio* GetFAudio(void)
	{
		return FAudioObj;
	}
};

typedef struct FAudioVoiceCallbackWithUserdata_
{
	FAudioVoiceCallback callback;
	void *userdata;
} FAudioVoiceCallbackWithUserdata;

class FAudioStream : public iTVPAudioStream
{
	FAudioDevice* Owner;
	FAudioSourceVoice* SourceVoiceObj;

	tTVPAudioStreamParam StreamParam;
	tjs_int AudioVolumeValue;
	tjs_int AudioBalanceValue;

	FAudioVoiceCallbackWithUserdata CallbackParam;

	StreamQueueCallback QueueCallback;
	void* UserData;

private:
	float CalcCommonVolume(float balance_sign) const
	{
		float level = (float)AudioVolumeValue / 100000.0f;
		if ((balance_sign != 0) && (AudioBalanceValue > 0))
		{
			float balance = (float)(100000 + (balance_sign * AudioBalanceValue)) / 100000.0f;
			level *= balance;
		}
		if (level < 0.0f)
		{
			level = 0.0f;
		}
		if (level > 1.0f)
		{
			level = 1.0f;
		}
		return level;
	}

	float CalcLeftVolume() const
	{
		return CalcCommonVolume(-1.0f);
	}
	float CalcRightVolume() const
	{
		return CalcCommonVolume(1.0f);
	}
	float CalcVolume() const
	{
		return CalcCommonVolume(0.0f);
	}

public:
	FAudioStream(FAudioDevice* parent, const tTVPAudioStreamParam& param)
	{
		Owner = parent;
		SourceVoiceObj = nullptr;
		AudioVolumeValue = iTVPAudioDevice::VOLUME_MAX;
		AudioBalanceValue = 0;
		QueueCallback = nullptr;
		UserData = nullptr;
		memset(&StreamParam, 0, sizeof(StreamParam));
		memset(&CallbackParam, 0, sizeof(CallbackParam));
		CallbackParam.callback.OnBufferEnd = OnBufferEndEntry;
		CallbackParam.userdata = (void *)this;
		uint16_t tag = FAUDIO_FORMAT_PCM;
		if (param.SampleType == astFloat32)
		{
			tag = FAUDIO_FORMAT_IEEE_FLOAT;
		}
		FAudioWaveFormatEx sourceWaveFormat;
		sourceWaveFormat.wFormatTag = tag;
		sourceWaveFormat.nChannels = param.Channels;
		sourceWaveFormat.nSamplesPerSec = param.SampleRate;
		sourceWaveFormat.wBitsPerSample = param.BitsPerSample;
		sourceWaveFormat.nBlockAlign = ( sourceWaveFormat.wBitsPerSample / 8 ) * sourceWaveFormat.nChannels;
		sourceWaveFormat.nAvgBytesPerSec = sourceWaveFormat.nSamplesPerSec * sourceWaveFormat.nBlockAlign;
		sourceWaveFormat.cbSize = 0;
		uint32_t hr;
		hr = FAudio_CreateSourceVoice(
			parent->GetFAudio(),
			&SourceVoiceObj,
			&sourceWaveFormat,
			0,
			FAUDIO_DEFAULT_FREQ_RATIO,
			&(CallbackParam.callback),
			nullptr,
			nullptr
		);
		if (hr != 0)
		{
			TVPThrowExceptionMessage(TJS_W("Failed to call FAudio_CreateSourceVoice"));
		}
		StreamParam = param;
	}

	virtual ~FAudioStream() override
	{
		if (SourceVoiceObj != nullptr)
		{
			FAudioVoice_DestroyVoice(SourceVoiceObj);
			SourceVoiceObj = nullptr;
		}
		if (Owner != nullptr)
		{
			Owner->DelStream(this);
			Owner = nullptr;
		}
	}

	virtual void SetCallback(StreamQueueCallback callback, void* user) override
	{
		QueueCallback = callback;
		UserData = user;
	}

	virtual void Enqueue(void *data, size_t size, bool last) override
	{
		if (SourceVoiceObj == nullptr)
		{
			return;
		}
		FAudioBuffer bufferDesc;
		memset(&bufferDesc, 0, sizeof(bufferDesc));
		bufferDesc.Flags = last ? FAUDIO_END_OF_STREAM : 0;
		bufferDesc.AudioBytes = size;
		bufferDesc.pAudioData = (uint8_t *)data;
		FAudioSourceVoice_SubmitSourceBuffer(SourceVoiceObj, &bufferDesc, nullptr);
	}
	virtual void ClearQueue() override
	{
		if (SourceVoiceObj == nullptr)
		{
			return;
		}
		FAudioSourceVoice_FlushSourceBuffers(SourceVoiceObj);
	}
	virtual tjs_uint32 GetQueuedCount() const override
	{
		if (SourceVoiceObj == nullptr)
		{
			return 0;
		}
		FAudioVoiceState state;
		FAudioSourceVoice_GetState(SourceVoiceObj, &state, FAUDIO_VOICE_NOSAMPLESPLAYED);
		return state.BuffersQueued;
	}
	virtual tjs_uint64 GetSamplesPlayed() const override
	{
		if (SourceVoiceObj == nullptr)
		{
			return 0;
		}
		FAudioVoiceState state;
		FAudioSourceVoice_GetState(SourceVoiceObj, &state, 0);
		return state.SamplesPlayed;
	}

	virtual void StartStream() override
	{
		if (SourceVoiceObj == nullptr)
		{
			return;
		}
		FAudioSourceVoice_Start(SourceVoiceObj, 0, FAUDIO_COMMIT_NOW);
	}
	virtual void StopStream() override
	{
		if (SourceVoiceObj == nullptr)
		{
			return;
		}
		FAudioSourceVoice_Stop(SourceVoiceObj, 0, FAUDIO_COMMIT_NOW);
		FAudioSourceVoice_FlushSourceBuffers(SourceVoiceObj);
	}
	virtual void AbortStream() override
	{
		if (SourceVoiceObj == nullptr)
		{
			return;
		}
		FAudioSourceVoice_Stop(SourceVoiceObj, 0, FAUDIO_COMMIT_NOW);
	}

	virtual void SetVolume(tjs_int vol) override
	{
		if (AudioVolumeValue == vol)
		{
			return;
		}
		if (vol > iTVPAudioDevice::VOLUME_MAX)
		{
			AudioVolumeValue = iTVPAudioDevice::VOLUME_MAX;
		}
		else if (vol < 0)
		{
			AudioVolumeValue = 0;
		}
		else
		{
			AudioVolumeValue = vol;
		} 

		SetVolumeToFAudio();
	}
	virtual tjs_int GetVolume() const override
	{
		return AudioVolumeValue;
	}
	virtual void SetPan(tjs_int pan) override
	{
		if (AudioBalanceValue == pan)
		{
			return;
		}
		if (pan < -100000)
		{
			AudioBalanceValue = -100000;
		}
		else if (pan > 100000)
		{
			AudioBalanceValue = 100000;
		}
		else
		{
			AudioBalanceValue = pan;
		}

		SetVolumeToFAudio();
	}
	virtual tjs_int GetPan() const override
	{
		return AudioBalanceValue;
	}
	virtual void SetFrequency(tjs_int freq) override
	{
		if (SourceVoiceObj == nullptr)
		{
			return;
		}
		float rate = (float)freq / (float)StreamParam.SampleRate;
		FAudioSourceVoice_SetFrequencyRatio(SourceVoiceObj, rate, FAUDIO_COMMIT_NOW);
	}
	virtual tjs_int GetFrequency() const override
	{
		if (SourceVoiceObj == nullptr)
		{
			return 0;
		}
		float rate = 1.0f;
		FAudioSourceVoice_GetFrequencyRatio(SourceVoiceObj, &rate);
		return (tjs_int)(StreamParam.SampleRate * rate);
	}

	void SetVolumeToFAudio()
	{
		if (SourceVoiceObj == nullptr)
		{
			return;
		}
		uint32_t count = StreamParam.Channels;
		if (count == 2)
		{
			float channels[2];
			channels[0] = CalcLeftVolume();
			channels[1] = CalcRightVolume();
			FAudioVoice_SetChannelVolumes(SourceVoiceObj, count, &channels[0], FAUDIO_COMMIT_NOW);
		}
		else if (count == 1)
		{
			FAudioVoice_SetVolume(SourceVoiceObj, CalcVolume(), FAUDIO_COMMIT_NOW);
		}
		else
		{
			float channels[count];
			for (uint32_t i = 0; i < count; i += 1)
			{
				channels[i] = CalcVolume();
			}
			FAudioVoice_SetChannelVolumes(SourceVoiceObj, count, &channels[0], FAUDIO_COMMIT_NOW);
		}
	}

	virtual void OnBufferEnd(void *pBufferContext)
	{
		if (QueueCallback == nullptr)
		{
			return;
		}
		QueueCallback(this, UserData);
	}

	static void OnBufferEndEntry(FAudioVoiceCallback *in_callback, void *pBufferContext)
	{
		FAudioVoiceCallbackWithUserdata *callback = (FAudioVoiceCallbackWithUserdata *)in_callback;
		if (callback != nullptr)
		{
			FAudioStream *thisobj = (FAudioStream *)(callback->userdata);
			if (thisobj != nullptr)
			{
				thisobj->OnBufferEnd(pBufferContext);
			}
		}
	}
};

iTVPAudioStream* FAudioDevice::CreateAudioStream(tTVPAudioStreamParam& param)
{
	FAudioStream* stream = new FAudioStream(this, param);
	AddStream(stream);
	return stream;
}
#endif

iTVPAudioDevice* TVPCreateAudioDevice_FAudio()
{
#ifdef TVP_FAUDIO_IMPLEMENT
	return new FAudioDevice();
#else
	return NULL;
#endif
}
