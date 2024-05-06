#include "EurorackWavetable.h"

void AudioSynthWavetableEurorack::update(void)
{
	audio_block_t* output, *fmModulationInputs, *voctModulationInputs, *warpModulationInputs, *positionModulationInputs;

	fmModulationInputs = receiveReadOnly(0);
	voctModulationInputs = receiveReadOnly(1);
	warpModulationInputs = receiveReadOnly(2);
	positionModulationInputs = receiveReadOnly(3);

	output = allocate();
	if (output == NULL)
	{
		if (fmModulationInputs != NULL) release(fmModulationInputs);
		if (voctModulationInputs != NULL) release(voctModulationInputs);
		if (warpModulationInputs != NULL) release(warpModulationInputs);
		if (positionModulationInputs != NULL) release(positionModulationInputs);
		Serial.println("Allocation was NULL");
		return;
	}

	int16_t fmModulationInput = 0;
	int16_t voctModulationInput = 0;
	int16_t warpModulationInput = 0;
	int16_t positionModulationInput = 0;

	for (uint32_t i = 0; i < AUDIO_BLOCK_SAMPLES; i++)
	{
		if (fmModulationInputs != NULL) fmModulationInput = fmModulationInputs->data[i];
		if (voctModulationInputs != NULL) voctModulationInput = voctModulationInputs->data[i];
		if (warpModulationInputs != NULL) warpModulationInput = warpModulationInputs->data[i];
		if (positionModulationInputs != NULL) positionModulationInput = positionModulationInputs->data[i];


		float modulatedPosition = constrain(positionModulationInput / (float)INT16_MAX, 0.0, 1.0);
		uint16_t wavetablePositionTruncated = (uint16_t)(modulatedPosition * (wavetableData.waveCount - 1));
		float positionInterpolation = (modulatedPosition * (wavetableData.waveCount - 1)) - wavetablePositionTruncated;

		uint16_t samplePositionTruncated = (uint16_t)samplePosition;


		float sampleLeft = wavetableData.sprite[wavetablePositionTruncated][(samplePositionTruncated) % WAVE_SAMPLE_COUNT] / (float)INT16_MAX;
		float sampleRight = wavetableData.sprite[wavetablePositionTruncated][(samplePositionTruncated + 1) % WAVE_SAMPLE_COUNT] / (float)INT16_MAX;
		float lowerWaveSample = linearInterpolation(sampleLeft, sampleRight, samplePosition - samplePositionTruncated);

		sampleLeft = wavetableData.sprite[min(wavetablePositionTruncated + 1, MAX_WAVES_PER_SPRITE - 1)][(samplePositionTruncated) % WAVE_SAMPLE_COUNT] / (float)INT16_MAX;
		sampleRight = wavetableData.sprite[min(wavetablePositionTruncated + 1, MAX_WAVES_PER_SPRITE - 1)][(samplePositionTruncated + 1) % WAVE_SAMPLE_COUNT] / (float)INT16_MAX;
		float upperWaveSample = linearInterpolation(sampleLeft, sampleRight, samplePosition - samplePositionTruncated);


		output->data[i] = round(constrain(lowpassFilter.processSingle(linearInterpolation(lowerWaveSample, upperWaveSample, positionInterpolation)) / 1.3, -1.0f, 1.0f) * INT16_MAX);

		relativeSamplePosition = fmod(relativeSamplePosition + (261.63 * pow(2, octave + (voctModulationInput / ((float)INT16_MAX / 12.0))) + ((fmModulationInput / (float)INT16_MAX) * fmRange)) / AUDIO_SAMPLE_RATE_EXACT, 1.0f);
		samplePosition = warpFunction(constrain(((warpModulationInput / (float)INT16_MAX)) * 0.5 + 0.5, 0.0, 1.0), relativeSamplePosition) * (WAVE_SAMPLE_COUNT);

	}

	if (positionModulationInputs != NULL) lastPositionModulationInput = positionModulationInputs->data[AUDIO_BLOCK_SAMPLES - 1] / (float)INT16_MAX;
	if (warpModulationInputs != NULL) lastWarpModulationInput = warpModulationInputs->data[AUDIO_BLOCK_SAMPLES - 1] / (float)INT16_MAX;


	if (fmModulationInputs != NULL) release(fmModulationInputs);
	if (voctModulationInputs != NULL) release(voctModulationInputs);
	if (warpModulationInputs != NULL) release(warpModulationInputs);
	if (positionModulationInputs != NULL) release(positionModulationInputs);

	transmit(output);
	release(output);
}

void AudioSynthWavetableEurorack::getWaveform(float position, float samples[], const uint16_t& size)
{
	uint16_t positionTruncated = position * (wavetableData.waveCount - 1);
	float positionInterpolation = position * (wavetableData.waveCount - 1) - positionTruncated;

	float samplePosition;

	for (int i = 0; i < size; i++)
	{
		samplePosition = warpFunction(constrain(lastWarpModulationInput * 0.5 + 0.5, 0.0, 1.0), i / (float)size) * (WAVE_SAMPLE_COUNT - 1);
		float lowerWaveSample = wavetableData.sprite[positionTruncated][round(samplePosition)] / (float)INT16_MAX;
		float upperWaveSample = wavetableData.sprite[min(positionTruncated + 1, MAX_WAVES_PER_SPRITE - 1)][round(samplePosition)] / (float)INT16_MAX;

		samples[i] = linearInterpolation(lowerWaveSample, upperWaveSample, positionInterpolation);
	}
}