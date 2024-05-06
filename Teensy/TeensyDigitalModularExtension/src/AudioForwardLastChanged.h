#pragma once
#include "Arduino.h"
#include "AudioStream.h"

class AudioForwardLastChanged : public AudioStream
{
	public:
		AudioForwardLastChanged() : AudioStream(2, inputQueueArray) {}
		void update()
		{
			audio_block_t* out = allocate();
			if (out == NULL)
			{
				Serial.println("Allocation was NULL");
				return;
			}

			audio_block_t* audioInput1 = receiveWritable(0);
			audio_block_t* audioInput2 = receiveWritable(1);

			if (audioInput1 == NULL)
			{
				if (audioInput2 != NULL)
				{
					out = audioInput2;
					transmit(out);
					release(out);
					release(audioInput2);
				}
				return;
			}


			if (audioInput2 == NULL)
			{
				if (audioInput1 != NULL)
				{
					out = audioInput1;
					transmit(out);
					release(out);
					release(audioInput1);
				}
				return;
			}


			for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++)
			{
				if (audioInput2->data[i] != channel2LastValue)
				{
					channel2LastValue = audioInput2->data[i];
					channelLastChanged = 2;
				}

				if(audioInput1->data[i] != channel1LastValue)
				{
					channel1LastValue = audioInput1->data[i];
					channelLastChanged = 1;
				}
			}

			if (channelLastChanged == 1)
			{
				for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++)
				{
					out->data[i] = audioInput1->data[i];
				}
			}
			else
			{
				for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++)
				{
					out->data[i] = audioInput2->data[i];
				}
			}

			transmit(out);
			release(out);
			release(audioInput1);
			release(audioInput2);
		}
	private:
		audio_block_t* inputQueueArray[2];
		int8_t channelLastChanged = 1;
		int16_t channel1LastValue = 0;
		int16_t channel2LastValue = 0;
};