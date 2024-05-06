#pragma once
#include <Arduino.h>
#include <AudioStream.h>

class AudioLED : public AudioStream
{

	public:
		AudioLED(uint8_t pin) : AudioStream(1, inputQueueArray)
		{
			pinMode(pin, OUTPUT);
			analogWriteFrequency(pin, 1000);
			analogWrite(pin, 0);
			LEDpin = pin;
		}
		virtual void update(void);

	private:
		audio_block_t* inputQueueArray[1];
		uint8_t LEDpin;

};