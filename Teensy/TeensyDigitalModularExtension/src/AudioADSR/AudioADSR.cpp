#include "AudioADSR.h"

void AudioADSR::update()
{
	audio_block_t* out = allocate();
	if (out == NULL)
	{
		Serial.println("Allocation was NULL");
		return;
	}


	audio_block_t* audioBlocks[8];

	for (int i = 0; i < 8; i++)
	{
		audioBlocks[i] = receiveReadOnly(i);
		if (audioBlocks[i] == NULL)
		{
			for (int j = 0; j < i; j++)
			{
				release(audioBlocks[j]);
			}
			release(out);
			Serial.println("Input was NULL");
			return;
		}
	}

	audio_block_t* triggerIn = audioBlocks[0];
	audio_block_t* attackTime = audioBlocks[1];
	audio_block_t* attackSlope = audioBlocks[2];
	audio_block_t* decayTime = audioBlocks[3];
	audio_block_t* decaySlope = audioBlocks[4];
	audio_block_t* sustainLevel = audioBlocks[5];
	audio_block_t* releaseTime = audioBlocks[6];
	audio_block_t* releaseSlope = audioBlocks[7];



	bool triggerActive = false;

	if (triggerIn != NULL)
	{
		for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++)
		{
			if (triggerIn->data[i])
			{
				triggerActive = true;
				break;
			}
		}
	}

	if (!triggerPreviouslyActive && triggerActive)
	{
		currentSection = ATTACK;
		sectionRelativePosition = 0.0f;
	}

	if (triggerPreviouslyActive && !triggerActive && (currentSection != RELEASE))
	{
		currentSection = RELEASE;
		sectionRelativePosition = 0.0f;
	}

	triggerPreviouslyActive = triggerActive;

	for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++)
	{
		switch (currentSection)
		{
			case ATTACK:
			{
				out->data[i] = (attackFunction(sectionRelativePosition, (float)attackSlope->data[i] / INT16_MAX) * (1- preAttackLevel) + preAttackLevel) * INT16_MAX;
				sectionRelativePosition += SECONDS_PER_SAMPLE / (timeFunction((float)attackTime->data[i] / INT16_MAX) * maxAttackTime);
				if (sectionRelativePosition > 1.0f)
				{
					sectionRelativePosition = 0.0f;
					currentSection = DECAY;
				}
				break;
			}

			case DECAY:
			{
				float relativSustainLevel = (float)sustainLevel->data[i] / INT16_MAX;
				out->data[i] = (decayFunction(sectionRelativePosition, (float)decaySlope->data[i] / INT16_MAX) * (1- relativSustainLevel) + relativSustainLevel) * INT16_MAX;
				sectionRelativePosition += SECONDS_PER_SAMPLE / (timeFunction((float)decayTime->data[i] / INT16_MAX) * maxDecayTime);
				if (sectionRelativePosition > 1.0f)
				{
					sectionRelativePosition = 0.0f;

					if(currentMode == ADSR)
					{
						currentSection = SUSTAIN;
					}
					else
					{
						preReleaseLevel = (float)out->data[i] / INT16_MAX;
						currentSection = RELEASE;
					}
				}
				break;
			}

			case SUSTAIN:
			{
				out->data[i] = sustainLevel->data[i];
				break;
			}

			case RELEASE:
			{
				out->data[i] = (decayFunction(sectionRelativePosition, (float)releaseSlope->data[i] / INT16_MAX) * preReleaseLevel) * INT16_MAX;
				sectionRelativePosition += SECONDS_PER_SAMPLE / (timeFunction((float)releaseTime->data[i] / INT16_MAX) * maxReleaseTime);
				if (sectionRelativePosition > 1.0f)
				{
					sectionRelativePosition = 0.0f;
					if(currentMode == LOOP)
					{
						currentSection = ATTACK;
						preAttackLevel = 0;
					}
					else
					{
						currentSection = STANDBY;
					}
				}
				break;
			}

			case STANDBY:
			{
				out->data[i] = 0;
				if(currentMode == LOOP)
				{
					currentSection = ATTACK;
					preAttackLevel = 0;
				}
				break;
			}
		}
	}

	if(currentSection != RELEASE)
		preReleaseLevel = (float)out->data[AUDIO_BLOCK_SAMPLES-1] / INT16_MAX;

	if (currentSection != ATTACK)
		preAttackLevel = (float)out->data[AUDIO_BLOCK_SAMPLES - 1] / INT16_MAX;

	transmit(out);
	release(out);

	if (triggerIn != NULL)
		release(triggerIn);

	if (attackTime != NULL)
		release(attackTime);

	if (attackSlope != NULL)
		release(attackSlope);

	if (decayTime != NULL)
		release(decayTime);

	if (decaySlope != NULL)
		release(decaySlope);

	if (sustainLevel != NULL)
		release(sustainLevel);

	if (releaseTime != NULL)
		release(releaseTime);

	if (releaseSlope != NULL)
		release(releaseSlope);
}

float AudioADSR::attackFunction(float position, float interpolation)
{
	if (interpolation < 0.0f)
	{
		interpolation = abs(interpolation);
		return interpolation * pow(2, 8.0f * (position - 1)) + (1- interpolation) * position;
	}
	else
	{
		return max(interpolation * ((log(position) / 5.5451f) + 1) + (1 - interpolation) * position, 0);
	}
}

float AudioADSR::decayFunction(float position, float interpolation)
{
	if (interpolation < 0.0f)
	{
		interpolation = abs(interpolation);
		return interpolation * pow(2, -8.0f * position) + (1 - interpolation) * (-position + 1);
	}
	else
	{
		if(position > 0.990f)
			return 0;
		return interpolation * ((log(-position+ 1) / 5.5451f) + 1) + (1 - interpolation) * (-position + 1);
	}
}

float AudioADSR::timeFunction(float time)
{
	return pow(time, 3);
}