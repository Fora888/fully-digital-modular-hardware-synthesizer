#include "SC16IS7xxInput.h"

void SC16IS7xxInput::updateIO()
{
	if (updateResponsibility)
	{
		AudioStream::update_all();
	}
}

void SC16IS7xxInput::update()
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

	//while (sc16is7Manager->getTransmitting()) {};
	noInterrupts();
	int8_t* temp = readyBuffer;
	readyBuffer = receivingBuffer;
	receivingBuffer = temp;
	uint16_t bytesReceived = sc16is7Manager->getBytesReceived(channel);
	sc16is7Manager->registerReceiveBuffer(channel, receivingBuffer);
	interrupts();

	for (int i = 0; i < AUDIO_BLOCK_SAMPLES * 3 && i < bytesReceived && currentSample < 128; i++)
	{
		int16_t received = readyBuffer[i];

		if (received & 0b01000000)
		{
			if (received & 0b10000000)
			{
				//order bits are 11
				//Serial.print("wth");
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
		outBlock->data[i] = outBlock->data[i - 1];
	}

	transmit(outBlock);
	release(outBlock);
}