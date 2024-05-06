#pragma once
#include "Arduino.h"
#include "AudioStream.h"
#include "SC16IS7xxManager.h"
#include "../AudioSerialIO/SerialInterruptManager.h"
#include "../AudioSerialIO/SerialAudioIO.h"

#define AUDIO_BAUD 2000000

extern SerialAudioManager audioInterruptManager;
class SC16IS7xxOutput : public AudioStream, public SerialAudioIO
{
public:
	SC16IS7xxOutput(SC16IS7xxManager* sc16is7Manager, uint8_t channel) : AudioStream(1, inputQueueArray)
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
	int8_t sendBufferA[AUDIO_BLOCK_SAMPLES * 3];
	int8_t sendBufferB[AUDIO_BLOCK_SAMPLES * 3];
	int8_t* sendingBuffer = sendBufferA;
	int8_t* preparingBuffer = sendBufferB;
	SC16IS7xxManager* sc16is7Manager;
	uint8_t channel = 0;
	audio_block_t* inputQueueArray[1];
};