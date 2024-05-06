#pragma once

#include "Arduino.h"
#include "AudioStream.h"
#include <SD.h>
#include "Math.h"
#include "RandomAccesSDWAVLoader.h"
#include "Bezier.h"
#define MAX_WAVES_PER_SPRITE 64
#define WAVE_SAMPLE_COUNT 1024

class BiquadFilter
{
	public:
		BiquadFilter(const float a0, const float a1, const float a2, const float b1, const float b2)
		{
			this->a0 = a0;
			this->a1 = a1;
			this->a2 = a2;
			this->b1 = b1;
			this->b2 = b2;
		};

		float processSingle(const float& sample)
		{
			out = a0 * sample + s1;
			s1 = s2 + a1 * sample - b1 * out;
			s2 = a2 * sample - b2 * out;
			return out;
		}
	private:
		float	s1 = 0,
			s2 = 0,
			out = 0;
		float a0, a1, a2, b1, b2;
};

class AudioSynthWavetableEurorack : public AudioStream
{
public:
	AudioSynthWavetableEurorack() : AudioStream(4, inputQueueArray) {};
	void update(void);
	void setOctave(const float& octave) { this->octave = octave; };
	void setFmRange(const float& newRange) {fmRange = newRange; };
	float position() { return constrain(lastPositionModulationInput, 0.0, 1.0); };
	void getWaveform(float position, float samples[], const uint16_t& size);

	struct WavetableData
	{
		uint16_t waveCount;
		int16_t(&sprite)[MAX_WAVES_PER_SPRITE][WAVE_SAMPLE_COUNT] = reinterpret_cast<int16_t(&)[MAX_WAVES_PER_SPRITE][WAVE_SAMPLE_COUNT]>(rawSamples);
		int16_t rawSamples[WAVE_SAMPLE_COUNT * MAX_WAVES_PER_SPRITE];
	};

	static float linearInterpolation(const float& y1, const float& y2, const float& x)
	{
		return y1 + x * (y2 - y1);
	}

	static float warpFunction(const float& a, const float& x)
	{
		return a * 2.0 * x * (1.0 - x) + sq(x); //Bernstein Polynomial
	};

	void begin()
	{
		initializeSD();
		loadNextFile();
	}

	void initializeSD()
	{
		wavLoader = new RandomAccesSDWAVLoader("/sprites/");
	}

	void loadFile(const float& relativeIndex)
	{
		uint32_t readSamples = wavLoader->loadFile(relativeIndex, wavetableData.rawSamples, MAX_WAVES_PER_SPRITE * WAVE_SAMPLE_COUNT);
		wavetableData.waveCount = readSamples / WAVE_SAMPLE_COUNT;	
	}

	void loadNextFile()
	{
		uint32_t readSamples = wavLoader->loadNextFile(wavetableData.rawSamples, MAX_WAVES_PER_SPRITE * WAVE_SAMPLE_COUNT);
		wavetableData.waveCount = readSamples / WAVE_SAMPLE_COUNT;
	}

	private:
		RandomAccesSDWAVLoader* wavLoader = NULL;
		audio_block_t* inputQueueArray[4];
		WavetableData wavetableData;
		BiquadFilter lowpassFilter{ 0.7197630406500103f, 1.4395260813000206f, 0.7197630406500103f, 1.4083834326624525f, 0.47066872993758846f};

		float samplePosition = 0.0, relativeSamplePosition = 0.0;
		float octave = 3;
		float fmRange = 500;

		float lastWarpModulationInput = 0.0;
		float lastPositionModulationInput = 0.0;
		uint32_t time = 0;

};
