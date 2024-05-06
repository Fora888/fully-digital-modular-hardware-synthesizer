#pragma once
#include "Arduino.h"
#include "AudioStream.h"
#include "SC16IS7xxManager.h"
#include "../AudioSerialIO/SerialInterruptManager.h"
#include "../AudioSerialIO/SerialAudioIO.h"

#define AUDIO_BAUD 2000000

extern SerialAudioManager audioInterruptManager;
class SC16IS7xxInput : public AudioStream, public SerialAudioIO
{
	public:
		SC16IS7xxInput(SC16IS7xxManager* sc16is7Manager, uint8_t channel) : AudioStream(0, NULL)
		{
			this->channel = channel;
			this->sc16is7Manager = sc16is7Manager;
			audioInterruptManager.addCallback(this);
			updateResponsibility = update_setup();
		}
		void updateIO();
		void update();
	private:
		bool updateResponsibility = false;
		int16_t carry = 0;
		int8_t receiveBufferA[AUDIO_BLOCK_SAMPLES * 3];
		int8_t receiveBufferB[AUDIO_BLOCK_SAMPLES * 3];
		int8_t* receivingBuffer = receiveBufferA;
		int8_t* readyBuffer = receiveBufferB;
		SC16IS7xxManager* sc16is7Manager;
		uint8_t channel = 0;

};