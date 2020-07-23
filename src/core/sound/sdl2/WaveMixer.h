#pragma once
#include "WaveIntf.h"

#ifdef __EMSCRIPTEN__
#define TVPAL_BUFFER_COUNT 16
#else
#define TVPAL_BUFFER_COUNT 4
#endif

struct ALSoundImpl;
void TVPInitDirectSound();
void TVPUninitDirectSound();

class iTVPSoundBuffer {
public:
	virtual ~iTVPSoundBuffer() {}
	virtual void Release() = 0;
	virtual void Play() = 0;
	virtual void Pause() = 0;
	virtual void Stop() = 0;
	virtual void Reset() = 0;
	virtual bool IsPlaying() = 0;
	virtual void SetVolume(float v) = 0;
	virtual float GetVolume() = 0;
	virtual void SetPan(float v) = 0;
	virtual float GetPan() = 0;
	virtual void AppendBuffer(const void *buf, unsigned int len) = 0;
	virtual bool IsBufferValid() = 0;
	virtual tjs_uint GetCurrentPlaySamples() = 0;
	virtual tjs_uint GetLatencySamples() = 0;
	virtual float GetLatencySeconds() { return 0; }
	virtual int GetRemainBuffers() = 0;
	virtual void SetPosition(float x, float y, float z) {}
};
iTVPSoundBuffer* TVPCreateSoundBuffer(tTVPWaveFormat &fmt, int bufcount);
