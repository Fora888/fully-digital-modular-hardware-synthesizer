#include "SC16IS7xxManager.h"

SC16IS7xxManager::SC16IS7xxManager(uint16_t CSPin)
{
	SPI.begin();
	pinMode(CSPin, OUTPUT);
	digitalWrite(CSPin, HIGH);
	this->CSPin = CSPin;
	delayMicroseconds(1);

	SPI.beginTransaction(SPISettings(15000000, MSBFIRST, SPI_MODE0));

	setupChannel(0);
	setupChannel(1);

	manager = this;
	event.attachImmediate(&SPIinterruptRoutine);
	interruptTimer.begin(interruptRoutine, 1000000.0f / ((AUDIO_SAMPLE_RATE_EXACT * 16) / 128.0));
}

void SC16IS7xxManager::registerReceiveBuffer(uint8_t channel, int8_t* buffer)
{
	if (channel == 0)
	{
		receiveBufferA->buffer = buffer;
		receiveBufferA->registered = true;
		receiveBufferA->position = 0;
	}
	else
	{
		receiveBufferB->buffer = buffer;
		receiveBufferB->registered = true;
		receiveBufferB->position = 0;
	}

}

void SC16IS7xxManager::registerSendBuffer(uint8_t channel, int8_t* buffer)
{
	if (channel == 0)
	{
		sendBufferA->buffer = buffer;
		sendBufferA->registered = true;
		sendBufferA->position = 0;
	}
	else
	{
		sendBufferB->buffer = buffer;
		sendBufferB->registered = true;
		sendBufferB->position = 0;
	}
	//Serial.println("New Buffer");
}

void SC16IS7xxManager::ManageFIFO()
{
	digitalWriteFast(CSPin, HIGH);
	delayNanoseconds(250);
	if (finishedTransmissionCount == 0)
	{
		if (receiveBufferA->position >= AUDIO_BLOCK_SAMPLES * 3 - 1 || !receiveBufferA->registered)
		{
			//Serial.println("Skipping");
			finishedTransmissionCount++;
			sentCharacters = 0;
		}
		else
		{
			transferBuffer(receiveBufferA, 0, 0);
			return;
		}
	}

	if (finishedTransmissionCount == 1)
	{
		receiveBufferA->position += sentCharacters;
		if (receiveBufferB->position >= AUDIO_BLOCK_SAMPLES * 3 - 1 || !receiveBufferB->registered)
		{
			finishedTransmissionCount++;
			sentCharacters = 0;
		}
		else
		{
			transferBuffer(receiveBufferB, 1, 0);
			return;
		}
	}

	if (finishedTransmissionCount == 2)
	{
		receiveBufferB->position += sentCharacters;

		if (sendBufferA->position >= AUDIO_BLOCK_SAMPLES * 3 - 1 || !sendBufferA->registered)
		{
			//Serial.println("Skipping");
			finishedTransmissionCount++;
			sentCharacters = 0;
		}
		else
		{
			transferBuffer(sendBufferA, 0, 1);
			return;
		}
	}

	if (finishedTransmissionCount == 3)
	{
		sendBufferA->position += sentCharacters;

		if (sendBufferB->position >= AUDIO_BLOCK_SAMPLES * 3 - 1 || !sendBufferB->registered)
		{
			finishedTransmissionCount++;
			sentCharacters = 0;
		}
		else
		{
			transferBuffer(sendBufferB, 1, 1);
			return;
		}
	}

	if (finishedTransmissionCount == 4)
	{
		sendBufferB->position += sentCharacters;

		finishedTransmissionCount = 0;
		isTransmitting = false;
	}
}

void SC16IS7xxManager::transferBuffer(UARTBuffer* buffer, uint8_t channel, bool send)
{
	uint16_t bufferSpaces = ((AUDIO_BLOCK_SAMPLES * 3) - buffer->position);// -1;
	uint8_t registerValue = readRegister(send ? 8 : 9, channel);

	sentCharacters = min(registerValue, bufferSpaces);

	digitalWriteFast(CSPin, LOW);
	delayNanoseconds(500);
	if (send)
	{
		SPI.transfer(0b00000000 | ((channel & 0b00000001) << 1));
		SPI.transfer(&buffer->buffer[buffer->position], nullptr, sentCharacters, event);
	}
	else
	{
		SPI.transfer(0b10000000 | ((channel & 0b00000001) << 1));
		SPI.transfer(nullptr, &buffer->buffer[buffer->position], sentCharacters, event);
	}
}

void SC16IS7xxManager::setupChannel(byte channel)
{
	//Disable all interrupts
	writeRegister(1, channel, 0b00000000);

	//Eneable enhanced Functions for TX empty interrupt
	writeRegister(3, channel, 0xBF);
	writeRegister(2, channel, 0b00010000);

	//Set clock divisor to 1
	writeRegister(3, channel, 0b10000011);
	writeRegister(0, channel, 1);
	writeRegister(1, channel, 0);
	writeRegister(3, channel, 0b00000011);

	//Eneable FIFO
	writeRegister(2, channel, 0b00000001);

	//Eneable Trigger Level Register
	writeRegister(4, channel, 0b00000100);

	//Eneable TLR Register
	writeRegister(4, channel, 0b00000100);

	//Set FIFO level to 48
	writeRegister(7, channel, 0b11001100);
}

void SC16IS7xxManager::writeRegister(uint8_t registerAddress, bool channel, byte value)
{
	noInterrupts();
	digitalWriteFast(CSPin, LOW);
	delayNanoseconds(500);

	uint8_t returnBuffer[2];
	uint8_t sendBuffer[2];
	sendBuffer[0] = 0 | ((registerAddress & 0b00001111) << 3) | ((channel & 0b00000001) << 1);
	sendBuffer[1] = value;
	SPI.transfer(sendBuffer, returnBuffer, 2);

	digitalWriteFast(CSPin, HIGH);
	delayNanoseconds(500);
	interrupts();
}

uint8_t SC16IS7xxManager::readRegister(uint8_t registerAddress, bool channel)
{
	noInterrupts();
	digitalWriteFast(CSPin, LOW);
	delayNanoseconds(500);

	uint8_t returnBuffer[2];
	uint8_t sendBuffer[2];
	sendBuffer[0] = (0b10000000 | ((registerAddress & 0b00001111) << 3) | ((channel & 0b00000001) << 1));
	sendBuffer[1] = 0;
	SPI.transfer(sendBuffer, returnBuffer, 2);

	digitalWriteFast(CSPin, HIGH);
	delayNanoseconds(500);
	interrupts();
	return returnBuffer[1];
}


SC16IS7xxManager* SC16IS7xxManager::manager = nullptr;