#include "AudioVCA.h"

void AudioVCA::update()
{
	audio_block_t* out = allocate();
	if (out == NULL)
	{
		Serial.println("Allocation was NULL");
		return;
	}

	audio_block_t* audioInput = receiveReadOnly(0);
	audio_block_t* voltageInput = receiveReadOnly(1);

	if (audioInput != NULL)
	{
		if (voltageInput != NULL)
		{
			for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++)
			{
				out->data[i] = (int16_t)(constrain(audioInput->data[i] * max((voltageInput->data[i] / (float)INT16_MAX), 0), INT16_MIN, INT16_MAX));
			}
		}
		else
		{
			for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++)
			{
				out->data[i] = audioInput->data[i];
			}
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

	if (voltageInput != NULL)
		release(voltageInput);
}