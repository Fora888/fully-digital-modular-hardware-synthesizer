#pragma once
#include "Arduino.h"
#include "AudioStream.h"

class AudioOffset : public AudioStream
{
	public:
		AudioOffset() : AudioStream(1, inputQueueArray) {}
		void update()
		{
			audio_block_t* out = allocate();
			if (out == NULL)
			{
				Serial.println("Allocation was NULL");
				return;
			}

			float offsetInterpolationSteps = (currentOffset - previousOffset) / AUDIO_BLOCK_SAMPLES;

			audio_block_t* audioInput = receiveReadOnly(0);

			if (audioInput != NULL)
			{
				for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++)
				{
					previousOffset += offsetInterpolationSteps;
					out->data[i] = constrain((int32_t)(previousOffset * INT16_MAX) + audioInput->data[i], INT16_MIN, INT16_MAX);
				}
			}
			else
			{
				for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++)
				{
					previousOffset += offsetInterpolationSteps;
					out->data[i] = previousOffset * INT16_MAX;
				}
			}

			transmit(out);
			release(out);

			if (audioInput != NULL)
				release(audioInput);
		};

		void offset(const float& newOffset) { currentOffset = constrain(newOffset, -1.0f, 1.0f);};
	private:
		audio_block_t* inputQueueArray[1];
		float currentOffset = 0.0f;
		float previousOffset = 0.0f;
};