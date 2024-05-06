#pragma once
#include "Arduino.h"
#include "AudioStream.h"
#include "SerialAudioIO.h"

class SerialAudioManager
{
	public:
		SerialAudioManager();
		static void addCallback(SerialAudioIO* serialIOPort);
	private:
		static void updateCallbacks();
		IntervalTimer interruptTimer;
		static uint16_t numberOfCallbacks;
		static SerialAudioIO* serialIOPorts[32];
};