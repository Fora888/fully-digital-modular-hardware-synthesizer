#pragma once
#include "Arduino.h"
#include "SPI.h"
#include "AudioStream.h"

#define AUDIO_BAUD 2000000

class SC16IS7xxManager
{
	public:
		SC16IS7xxManager(uint16_t CSPin);

		void registerReceiveBuffer(uint8_t channel, int8_t* buffer);

		void registerSendBuffer(uint8_t channel, int8_t* buffer);

		uint16_t getBytesReceived(uint8_t channel)
		{
			return channel == 0 ? receiveBufferA->position : receiveBufferB->position;
		}

		bool getTransmitting()
		{
			return isTransmitting;
		}

		void getInterruptStatus()
		{
			Serial.println();
			Serial.println(readRegister(1, 0), BIN);
			Serial.println(readRegister(2, 0), BIN);
			Serial.println(readRegister(3, 0), BIN);
			Serial.println(readRegister(4, 0), BIN);
			Serial.println(readRegister(5, 0), BIN);
			Serial.println(readRegister(6, 0), BIN);
			Serial.println(readRegister(7, 0), BIN);
			Serial.println(readRegister(8, 0), BIN);
			Serial.println(readRegister(9, 0), BIN);

			Serial.println();
		}

	private:		
		struct UARTBuffer 
		{
			int8_t* buffer;
			bool registered = false;
			uint16_t position = 0;
		};

		static SC16IS7xxManager* manager;
		bool isTransmitting = false;
		bool SPIlock = false;
		EventResponder event;
		uint8_t finishedTransmissionCount = 0;
		uint16_t CSPin = 0;
		uint16_t sentCharacters = 0;

		static void SPIinterruptRoutine(EventResponderRef event_responder)
		{
				//Serial.println("SPI");
				manager->finishedTransmissionCount++;
				manager->ManageFIFO();
		}

		void ManageFIFO();

		void transferBuffer(UARTBuffer* buffer, uint8_t channel, bool send);

		static void interruptRoutine()
		{
			//Serial.println("PIN");
			if (manager->isTransmitting == false)
			{
				manager->isTransmitting = true;
				manager->ManageFIFO();
			}
		}

		void setupChannel(byte channel);
		
		void writeRegister(uint8_t registerAddress, bool channel, byte value);

		uint8_t readRegister(uint8_t registerAddress, bool channel);

		UARTBuffer* receiveBufferA = new UARTBuffer();
		UARTBuffer* receiveBufferB = new UARTBuffer();
		UARTBuffer* sendBufferA = new UARTBuffer();
		UARTBuffer* sendBufferB = new UARTBuffer();
		IntervalTimer interruptTimer;


};