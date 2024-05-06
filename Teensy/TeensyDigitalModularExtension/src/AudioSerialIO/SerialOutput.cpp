#include "SerialOutput.h"

extern SerialAudioManager audioInterruptManager;
AudioOutputSerial::AudioOutputSerial(HardwareSerial* port) : AudioStream(1, inputQueueArray)
{
	serialPort = port;
	serialPort->addMemoryForWrite(new uint8_t[AUDIO_BLOCK_SAMPLES * 3], AUDIO_BLOCK_SAMPLES * 3);
	serialPort->begin(AUDIO_BAUD);
	audioInterruptManager.addCallback(this);
	updateResponsibility = update_setup();
}

void AudioOutputSerial::updateIO()
{
	if(updateResponsibility)
	{
		AudioStream::update_all();
	}

}

void AudioOutputSerial::update()
{
	audio_block_t* block = receiveReadOnly(0);
	if (block != NULL)
	{
		for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++)
		{
			serialPort->write((block->data[i] & 0b0000000000111111) | 0b00000000);
			serialPort->write(((block->data[i] & 0b0000111111000000) >> 6) | 0b01000000);
			serialPort->write(((block->data[i] & 0b1111000000000000) >> 12) | 0b10000000);
		}
	}
	else
	{
		for (int i = 0; i < AUDIO_BLOCK_SAMPLES * 3; i++)
		{
			serialPort->write(0);
		}
	}

	if(block != NULL)
		release(block);
}

