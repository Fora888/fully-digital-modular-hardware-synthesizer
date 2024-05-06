#include "SerialInterruptManager.h"

SerialAudioManager::SerialAudioManager()
{
	//while (!Serial) {};
	//Serial.println("amogus");
	interruptTimer.priority(129);
	interruptTimer.begin(updateCallbacks, 1000000.0f / (AUDIO_SAMPLE_RATE_EXACT / 128.0));
}

void SerialAudioManager::addCallback(SerialAudioIO* serialIOPort)
{
	serialIOPorts[numberOfCallbacks] = serialIOPort;
	numberOfCallbacks++;
}

void SerialAudioManager::updateCallbacks()
{
	for (uint16_t i = 0; i < numberOfCallbacks; i++)
	{
		serialIOPorts[i]->updateIO();
	}

}

SerialAudioManager audioInterruptManager;
SerialAudioIO* SerialAudioManager::serialIOPorts[32];
uint16_t SerialAudioManager::numberOfCallbacks = 0;