#pragma once
#include "Arduino.h"
#include "AudioStream.h"

//0 is audio input, 1 is voltage input
class AudioVCA : public AudioStream
{
	public:
		AudioVCA() : AudioStream(2, inputQueueArray) {}
		virtual void update();
	private:
		audio_block_t* inputQueueArray[2];
};
