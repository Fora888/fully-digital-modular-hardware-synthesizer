#include <Audio.h>
#include <TeensyDigitalModularExtension.h>
#include <ButtonsAndKnobs.h>


#define ATTACK_LED_PIN 12
#define DECAY_SUSTAIN_LED_PIN 13
#define RELEASE_LED_PIN 33

// Audio Objects

AudioADSR     adsrGenerator;

AudioInputSerial                triggerIn(&Serial1);

AudioInputSerial                attackTimeModIn(&Serial2);
AudioAmplifier                  attackTimeModScale;
AudioOffset                     attackTimeOffset;
AudioOffset                     attackSlopeOffset;


AudioInputSerial                decayTimeModIn(&Serial3);
AudioAmplifier                  decayTimeModScale;
AudioOffset                     decayTimeOffset;
AudioOffset                     decaySlopeOffset;

AudioOffset                     sustainOffset;

AudioInputSerial                releaseTimeModIn(&Serial4);
AudioAmplifier                  releaseTimeModScale;
AudioOffset                     releaseTimeOffset;
AudioOffset                     releaseSlopeOffset;

AudioAmplifier                  outputScale;
AudioInvert                     outputInverter;

AudioOutputSerial               ADSROutput(&Serial1);
AudioOutputSerial               outputInverted(&Serial2);

AudioLED                      attackIndicator(ATTACK_LED_PIN);
AudioLED                      decaySustainIndicator(DECAY_SUSTAIN_LED_PIN);
AudioLED                      releaseIndicator(RELEASE_LED_PIN);

AudioSynthWaveformDc          zeroSource;   


//Audio Routing

AudioConnection           triggerToAdsrGenerator(triggerIn, 0, adsrGenerator, 0);

AudioConnection           attackTimeModInToAttackTimeModScale(attackTimeModIn, 0, attackTimeModScale, 0);
AudioConnection           attackTimeModScaleToAttackTimeOffset(attackTimeModScale, 0, attackTimeOffset, 0);
AudioConnection           attackTimeOffsetToAdsrGenerator(attackTimeOffset, 0, adsrGenerator, 1);
AudioConnection           attackSlopeOffsetToAdsrGenerator(attackSlopeOffset, 0, adsrGenerator, 2);

AudioConnection           decayTimeModInToDecayTimeModScale(decayTimeModIn, 0, decayTimeModScale, 0);
AudioConnection           decayTimeModScaleToDecayTimeOffset(decayTimeModScale, 0, decayTimeOffset, 0); 
AudioConnection           decayTimeOffsetToAdsrGenerator(decayTimeOffset, 0, adsrGenerator, 3);
AudioConnection           decaySlopeOffsetToAdsrGenerator(decaySlopeOffset, 0, adsrGenerator, 4);

AudioConnection           sustainOffsetToAdsrGenerator(sustainOffset, 0, adsrGenerator, 5);

AudioConnection           releaseTimeModInToReleaseTimeModScale(releaseTimeModIn, 0, releaseTimeModScale, 0);
AudioConnection           releaseTimeModScaleToReleaseTimeOffset(releaseTimeModScale, 0, releaseTimeOffset, 0); 
AudioConnection           releaseTimeOffsetToAdsrGenerator(releaseTimeOffset, 0, adsrGenerator, 6);
AudioConnection           releaseSlopeOffsetToAdsrGenerator(releaseSlopeOffset, 0, adsrGenerator, 7);


AudioConnection           adsrGeneratorToOutputScale(adsrGenerator, 0, outputScale, 0);
AudioConnection           outputScaleToOutput(outputScale, 0, ADSROutput, 0);

AudioConnection           outputScaleToInverter(outputScale, 0, outputInverter, 0);
AudioConnection           inverterToOutputInverted(outputInverter, 0, outputInverted, 0);

AudioConnection           toAttackIndicator(zeroSource, attackIndicator);
AudioConnection           toDecaySustainIndicator(zeroSource, decaySustainIndicator);
AudioConnection           toReleaseIndicator(zeroSource, releaseIndicator);


// Pot and Button Pins

#define ATTACK_TIME_MOD_OFFSET_DIGITAL_PIN 4
#define ATTACK_TIME_MOD_OFFSET_ANALOG_PIN 18
#define ATTACK_TIME_MOD_SCALE_DIGITAL_PIN 3
#define ATTACK_TIME_MOD_SCALE_ANALOG_PIN 17
#define ATTACK_SLOPE_OFFSET_DIGITAL_PIN 2
#define ATTACK_SLOPE_OFFSET_ANALOG_PIN 14

#define DECAY_TIME_MOD_OFFSET_DIGITAL_PIN 10
#define DECAY_TIME_MOD_OFFSET_ANALOG_PIN 22
#define DECAY_TIME_MOD_SCALE_DIGITAL_PIN 9
#define DECAY_TIME_MOD_SCALE_ANALOG_PIN 21
#define DECAY_SLOPE_OFFSET_DIGITAL_PIN 5
#define DECAY_SLOPE_OFFSET_ANALOG_PIN 19

#define SUSTAIN_OFFSET_DIGITAL_PIN 6
#define SUSTAIN_OFFSET_ANALOG_PIN 20

#define RELEASE_TIME_MOD_OFFSET_DIGITAL_PIN 28
#define RELEASE_TIME_MOD_OFFSET_ANALOG_PIN 26
#define RELEASE_TIME_MOD_SCALE_DIGITAL_PIN 29
#define RELEASE_TIME_MOD_SCALE_ANALOG_PIN 25
#define RELEASE_SLOPE_OFFSET_DIGITAL_PIN 27
#define RELEASE_SLOPE_OFFSET_ANALOG_PIN 24

#define OUTPUT_SCALE_DIGITAL_PIN 11
#define OUTPUT_SCALE_ANALOG_PIN 23

#define MODE_SWITCH_DOWN_DIGITAL_PIN 31
#define MODE_SWITCH_UP_DIGITAL_PIN 30


//Pot and Button Objects

Potentiometer attackTimeModOffsetPot(ATTACK_TIME_MOD_OFFSET_DIGITAL_PIN,ATTACK_TIME_MOD_OFFSET_ANALOG_PIN);
Potentiometer attackTimeModScalePot(ATTACK_TIME_MOD_SCALE_DIGITAL_PIN,ATTACK_TIME_MOD_SCALE_DIGITAL_PIN);
Potentiometer attackSlopeOffsetPot(ATTACK_SLOPE_OFFSET_DIGITAL_PIN, ATTACK_SLOPE_OFFSET_ANALOG_PIN);

Potentiometer decayTimeModOffsetPot(DECAY_TIME_MOD_OFFSET_DIGITAL_PIN, DECAY_TIME_MOD_OFFSET_ANALOG_PIN);
Potentiometer decayTimeModScalePot(DECAY_TIME_MOD_SCALE_DIGITAL_PIN, DECAY_TIME_MOD_SCALE_ANALOG_PIN);
Potentiometer decaySlopeOffsetPot(DECAY_SLOPE_OFFSET_DIGITAL_PIN, DECAY_SLOPE_OFFSET_ANALOG_PIN);

Potentiometer sustainOffsetPot(SUSTAIN_OFFSET_DIGITAL_PIN, SUSTAIN_OFFSET_ANALOG_PIN);

Potentiometer releaseTimeModOffsetPot(RELEASE_TIME_MOD_OFFSET_DIGITAL_PIN, RELEASE_TIME_MOD_OFFSET_ANALOG_PIN);
Potentiometer releaseTimeModScalePot(RELEASE_TIME_MOD_SCALE_DIGITAL_PIN, RELEASE_TIME_MOD_SCALE_ANALOG_PIN);
Potentiometer releaseSlopeOffsetPot(RELEASE_SLOPE_OFFSET_DIGITAL_PIN, RELEASE_SLOPE_OFFSET_ANALOG_PIN);

Potentiometer outputScalePot(11,23);



void setup() 
{
  pinMode(MODE_SWITCH_DOWN_DIGITAL_PIN, INPUT_PULLUP);
  pinMode(MODE_SWITCH_UP_DIGITAL_PIN, INPUT_PULLUP);
  AudioMemory(20);

}

void loop() 
{

  //Read Pots
  
  float attackTimeModOffsetReading = attackTimeModOffsetPot.read();
  float attackSlopeOffsetReading = -(attackSlopeOffsetPot.read() * 2.0f - 1.0f);
  float attackTimeModScaleReading = attackTimeModScalePot.read();
  
  float decayTimeModOffsetReading = decayTimeModOffsetPot.read();
  float decaySlopeOffsetReading = decaySlopeOffsetPot.read() * 2.0f  - 1.0f;
  float decayTimeModScaleReading = decayTimeModScalePot.read();
  
  float sustainOffsetReading = sustainOffsetPot.read();
  
  float releaseTimeModOffsetReading = releaseTimeModOffsetPot.read();
  float releaseSlopeOffsetReading = releaseSlopeOffsetPot.read() * 2.0f - 1.0f;
  
  float releaseTimeModScaleReading = releaseTimeModScalePot.read();
  
  float outputScaleReading = outputScalePot.read();

  //Apply new values

  AudioNoInterrupts();
  attackTimeOffset.offset(attackTimeModOffsetReading);
  attackSlopeOffset.offset(attackSlopeOffsetReading);
  attackTimeModScale.gain(attackTimeModScaleReading);
  
  decayTimeOffset.offset(decayTimeModOffsetReading);
  decaySlopeOffset.offset(decaySlopeOffsetReading);
  decayTimeModScale.gain(decayTimeModScaleReading);
  
  sustainOffset.offset(sustainOffsetReading);
  
  releaseTimeOffset.offset(releaseTimeModOffsetReading);
  releaseSlopeOffset.offset(releaseSlopeOffsetReading);
  releaseTimeModScale.gain(releaseTimeModScaleReading);
  
  outputScale.gain(outputScaleReading);
  
  if(!digitalRead(MODE_SWITCH_DOWN_DIGITAL_PIN))
  {
    adsrGenerator.setMode(AudioADSR::Mode::LOOP);
  }
  else if(!digitalRead(MODE_SWITCH_UP_DIGITAL_PIN))
  {
    adsrGenerator.setMode(AudioADSR::Mode::ADR);
  }
  else
  {
    adsrGenerator.setMode(AudioADSR::Mode::ADSR);
  }

  switch(adsrGenerator.getCurrentSection())
  {
    case AudioADSR::Section::ATTACK:
      toAttackIndicator.disconnect();
      toDecaySustainIndicator.disconnect();
      toReleaseIndicator.disconnect();
      toAttackIndicator.connect(adsrGenerator, attackIndicator);
      toDecaySustainIndicator.connect(zeroSource, decaySustainIndicator);
      toReleaseIndicator.connect(zeroSource, releaseIndicator);
      break;

    case AudioADSR::Section::DECAY:
    case AudioADSR::Section::SUSTAIN:
      toAttackIndicator.disconnect();
      toDecaySustainIndicator.disconnect();
      toReleaseIndicator.disconnect();
      toAttackIndicator.connect(zeroSource, attackIndicator);
      toDecaySustainIndicator.connect(adsrGenerator, decaySustainIndicator);
      toReleaseIndicator.connect(zeroSource, releaseIndicator);
      break;

    case AudioADSR::Section::RELEASE:
      toAttackIndicator.disconnect();
      toDecaySustainIndicator.disconnect();
      toReleaseIndicator.disconnect();
      toAttackIndicator.connect(zeroSource, attackIndicator);
      toDecaySustainIndicator.connect(zeroSource, decaySustainIndicator);
      toReleaseIndicator.connect(adsrGenerator, releaseIndicator);
      break;

    case AudioADSR::Section::STANDBY:
      toAttackIndicator.disconnect();
      toDecaySustainIndicator.disconnect();
      toReleaseIndicator.disconnect();
      toAttackIndicator.connect(zeroSource, attackIndicator);
      toDecaySustainIndicator.connect(zeroSource, decaySustainIndicator);
      toReleaseIndicator.connect(zeroSource, releaseIndicator);
      break; 
  }
  
  AudioInterrupts();
  
}
