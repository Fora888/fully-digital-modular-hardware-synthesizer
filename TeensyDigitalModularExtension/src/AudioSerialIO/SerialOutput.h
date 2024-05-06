#pragma once
#include "Time.h"
#include "Arduino.h"
#include "AudioStream.h"
#include "SerialInterruptManager.h"
#include "SerialAudioIO.h"

#define AUDIO_BAUD 2000000

//extern SerialAudioManager audioInterruptManager;
class AudioOutputSerial : public AudioStream, public SerialAudioIO
{
	public:
		AudioOutputSerial(HardwareSerial* port);
		void updateIO();
		void update();
	private:
		HardwareSerial* serialPort;
		bool updateResponsibility = false;
		audio_block_t* inputQueueArray[1];
};