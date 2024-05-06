#pragma once
#include "Arduino.h"
#include "AudioStream.h"

class AudioInvert : public AudioStream
{
public:
	AudioInvert() : AudioStream(1, inputQueueArray) {}
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
				out->data[i] = constrain((int32_t) -audioInput->data[i], INT16_MIN, INT16_MAX);
			}
		}
		else
		{
			for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++)
			{
				out->data[i] = 0;
			}
		}

		transmit(out);
		release(out);

		if (audioInput != NULL)
			release(audioInput);
	};
private:
	audio_block_t* inputQueueArray[1];
};