#pragma once
#include "Arduino.h"
#include "AudioStream.h"
#include "SerialInterruptManager.h"
#include "SerialAudioIO.h"

#define AUDIO_BAUD 2000000

//extern SerialAudioManager audioInterruptManager;
class AudioInputSerial : public AudioStream, public SerialAudioIO
{
	public:
		AudioInputSerial(HardwareSerial* port);
		void updateIO();
		void update();
	private:
		HardwareSerial* serialPort;
		bool updateResponsibility = false;
		int16_t carry = 0;
};