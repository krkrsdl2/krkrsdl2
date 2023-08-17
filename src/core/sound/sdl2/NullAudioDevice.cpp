/* SPDX-License-Identifier: MIT */
/* Copyright (c) Kirikiri SDL2 Developers */

#include "tjsCommHead.h"

#include <vector>
#include "AudioDevice.h"

class NullAudioStream;
class NullAudioDevice : public iTVPAudioDevice
{
	tjs_int Volume;

	std::vector<NullAudioStream*> Children;
public:
	NullAudioDevice()
	{
		Volume = 100000;
	}

	virtual ~NullAudioDevice() override
	{
	}

	virtual void Initialize(tTVPAudioInitParam& param) override
	{
	}

	virtual void Uninitialize() override
	{
		while (Children.size() != 0)
		{
			auto i = Children.begin();
			delete *i;
		}
	}

	virtual iTVPAudioStream* CreateAudioStream(tTVPAudioStreamParam& param) override;

	virtual void SetMasterVolume(tjs_int vol) override
	{
		if (Volume == vol)
		{
			return;
		}
		Volume = vol;
	}
	virtual tjs_int GetMasterVolume() const override
	{
		return Volume;
	}

	void AddStream(NullAudioStream* stream)
	{
		Children.push_back(stream);
	}

	void DelStream(NullAudioStream* stream)
	{
		auto i = std::remove(Children.begin(), Children.end(), stream);
		Children.erase(i, Children.end());
	}
};

class NullAudioStream : public iTVPAudioStream
{
	NullAudioDevice* Owner;

	tjs_int AudioVolumeValue;
	tjs_int AudioBalanceValue;
	tjs_int AudioFrequencyValue;

public:
	NullAudioStream(NullAudioDevice* parent, const tTVPAudioStreamParam& param)
	{
		Owner = parent;
		AudioVolumeValue = iTVPAudioDevice::VOLUME_MAX;
		AudioBalanceValue = 0;
		AudioFrequencyValue = 0;
	}

	virtual ~NullAudioStream() override
	{
		if (Owner != nullptr)
		{
			Owner->DelStream(this);
			Owner = nullptr;
		}
	}

	virtual void SetCallback(StreamQueueCallback callback, void* user) override
	{
	}

	virtual void Enqueue(void *data, size_t size, bool last) override
	{
		// TODO: increment variable returned by GetSamplesPlayed
	}
	virtual void ClearQueue() override
	{
	}
	virtual tjs_uint32 GetQueuedCount() const override
	{
		return 0;
	}
	virtual tjs_uint64 GetSamplesPlayed() const override
	{
		// TODO: return state adjusted by Enqueue
		return 0;
	}

	virtual void StartStream() override
	{
	}
	virtual void StopStream() override
	{
	}
	virtual void AbortStream() override
	{
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
	}
	virtual tjs_int GetPan() const override
	{
		return AudioBalanceValue;
	}
	virtual void SetFrequency(tjs_int freq) override
	{
		AudioFrequencyValue = freq;
	}
	virtual tjs_int GetFrequency() const override
	{
		return AudioFrequencyValue;
	}
};

iTVPAudioStream* NullAudioDevice::CreateAudioStream(tTVPAudioStreamParam& param)
{
	NullAudioStream* stream = new NullAudioStream(this, param);
	AddStream(stream);
	return stream;
}

iTVPAudioDevice* TVPCreateAudioDevice_Null()
{
	return new NullAudioDevice();
}
