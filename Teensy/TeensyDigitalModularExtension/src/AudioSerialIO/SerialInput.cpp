#include "SerialInput.h"

extern SerialAudioManager audioInterruptManager;
AudioInputSerial::AudioInputSerial(HardwareSerial* port) : AudioStream(0, NULL)
{
	serialPort = port;
	serialPort->addMemoryForRead(new uint8_t[AUDIO_BLOCK_SAMPLES * 3], AUDIO_BLOCK_SAMPLES * 3);
	serialPort->begin(AUDIO_BAUD);
	::audioInterruptManager.addCallback(this);
	updateResponsibility = update_setup();
}


void AudioInputSerial::updateIO()
{
	if (updateResponsibility)
	{
		AudioStream::update_all();
	}
}

void AudioInputSerial::update()
{
	audio_block_t* outBlock = allocate();
	if (outBlock == NULL)
	{
		Serial.println("Allocation was NULL");
		return;
	}

	int16_t currentSample = 0;
	for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++)
		outBlock->data[i] = 0;

	outBlock->data[0] |= carry;
	carry = 0;

	for (int i = 0; i < AUDIO_BLOCK_SAMPLES * 3 && serialPort->available() && currentSample < 128; i++)
	{
		int16_t received = serialPort->read();

		if (received & 0b01000000)
		{
			if (received & 0b10000000)
			{
				//order bits are 11
				Serial.print("Received invalid packet");
			}
			else
			{			
				//order bits are 01
				if (currentSample < 128)
				{
					outBlock->data[currentSample] |= (received << 6) & 0b0000111111000000;
				}
				else
				{
					carry |= (received << 6) & 0b0000111111000000;;
				}
			}
		}
		else
		{
			if (received & 0b10000000)
			{
				//order bits are 10
				outBlock->data[currentSample] |= (received << 12) & 0b1111000000000000;
				currentSample++;
			}
			else
			{
				//order bits are 00
				if (currentSample < 128)
				{
					outBlock->data[currentSample] |= (received & 0b0000000000111111);
				}
				else
				{
					carry |= (received & 0b0000000000111111);
				}
			}
		}
	}

	if (currentSample == 0)
	{
		outBlock->data[0] = 0;
		currentSample++;
	}

	for (int i = currentSample; i < AUDIO_BLOCK_SAMPLES; i++)
	{
		outBlock->data[i] = outBlock->data[i-1];
	}

	transmit(outBlock);
	release(outBlock);
}