#pragma once
#include "Arduino.h"
#include "AudioStream.h"
#define sgn(x) ((x) < 0 ? -1 : ((x) > 0 ? 1 : 0))

//0 is audio input, 1 is voltage input
class AudioLinearToExpDB : public AudioStream
{
public:
	AudioLinearToExpDB() : AudioStream(1, inputQueueArray) {}
	void update()
	{
		audio_block_t* out = allocate();
		if (out == NULL)
		{
			Serial.println("Allocation was NULL");
			return;
		}

		audio_block_t* audioInput = receiveReadOnly(0);
		if (audioInput != NULL)
		{
			for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++)
			{
				out->data[i] = (int16_t)(strength * pow(10.0, (7.0 * ((abs(audioInput->data[i]) / (float)INT16_MAX) - 1.0)) / 2.0) * INT16_MAX * sgn(audioInput->data[i]) + (1.0 - strength) * audioInput->data[i]);
			}
		}

		transmit(out);
		release(out);

		if (audioInput != NULL)
			release(audioInput);
	};
	void setStrength(float& newStrength) { strength = newStrength; };
private:
	audio_block_t* inputQueueArray[1];
	float strength = 1.0;
};