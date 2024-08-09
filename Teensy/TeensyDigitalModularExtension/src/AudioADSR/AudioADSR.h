#pragma once
#include "Arduino.h"
#include "AudioStream.h"
#include <math.h>

#define SECONDS_PER_SAMPLE 1.0 / 44100.0

//0 is audio input, 1 is voltage input
class AudioADSR : public AudioStream
{
public:
	AudioADSR() : AudioStream(8, inputQueueArray) {}
	virtual void update();
	enum Mode { ADSR, ADR, LOOP };
	enum Section { ATTACK, DECAY, SUSTAIN, RELEASE, STANDBY };
	void setMode(Mode newMode) {currentMode = newMode;};
	Section getCurrentSection() {return currentSection;};
private:
	audio_block_t* inputQueueArray[8];
	float maxAttackTime = 20.0f; 
	float maxDecayTime = 20.0f; 
	float maxReleaseTime = 20.0f;

	float preReleaseLevel = 0.0f;
	float preAttackLevel = 0.0f;

	Section currentSection = ATTACK;
	float sectionRelativePosition = 0.0f;

	Mode currentMode = ADSR;

	bool triggerPreviouslyActive = false;

	float attackFunction(float position, float interpolation);

	float decayFunction(float position, float interpolation);

	float timeFunction(float time);
};