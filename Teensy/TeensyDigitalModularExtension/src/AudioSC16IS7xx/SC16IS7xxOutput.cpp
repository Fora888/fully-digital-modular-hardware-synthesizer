#include "SC16IS7xxOutput.h"

void SC16IS7xxOutput::updateIO()
{
	if (updateResponsibility)
	{
		AudioStream::update_all();
	}

}

void SC16IS7xxOutput::update()
{
	//Serial.println("Update");
	audio_block_t* block = receiveReadOnly(0);
	if (block != NULL)
	{
		for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++)
		{
			preparingBuffer[i * 3] = ((block->data[i] & 0b0000000000111111) | 0b00000000);
			preparingBuffer[i * 3 + 1] = (((block->data[i] & 0b0000111111000000) >> 6) | 0b01000000);
			preparingBuffer[i * 3 + 2] = (((block->data[i] & 0b1111000000000000) >> 12) | 0b10000000);
		}
	}
	else
	{
		
		for (int i = 0; i < AUDIO_BLOCK_SAMPLES * 3; i++)
		{
			preparingBuffer[i] = 0xff;
		}
	}

	noInterrupts();
	int8_t* temp = sendingBuffer;
	sendingBuffer = preparingBuffer;
	preparingBuffer = temp;
	sc16is7Manager->registerSendBuffer(channel, sendingBuffer);
	interrupts();

	if (block != NULL)
		release(block);
}