#include<ButtonsAndKnobs.h>
#include<TeensyDigitalModularExtension.h>
#include <Audio.h>


// Audio Objects

AudioVCA                    vca1;
AudioInputSerial            audioIn1(&Serial1); 
AudioInputSerial            gainModIn1(&Serial2);
AudioAmplifier              gainModScale1;
AudioLinearToExpDB          linearToExp1;
AudioOffset                 gainModOffset1;
AudioOutputSerial           output1(&Serial1);

AudioVCA                    vca2;
AudioInputSerial            audioIn2(&Serial3); 
AudioInputSerial            gainModIn2(&Serial4); 
AudioAmplifier              gainModScale2;
AudioLinearToExpDB          linearToExp2;
AudioOffset                 gainModOffset2;
AudioMixer4                 mixer2;
AudioOutputSerial           output2(&Serial2);

AudioVCA                    vca3;
AudioInputSerial            audioIn3(&Serial5); 
AudioInputSerial            gainModIn3(&Serial6);
AudioAmplifier              gainModScale3;
AudioLinearToExpDB          linearToExp3;
AudioOffset                 gainModOffset3;
AudioMixer4                 mixer3;
AudioOutputSerial           output3(&Serial3);


//Audio Routing

AudioConnection          audioToVCA1(audioIn1, 0, vca1, 0);
AudioConnection          gainModToScale1(gainModIn1, 0, gainModScale1, 0);
AudioConnection          gainModScaleToExp1(gainModScale1, 0, linearToExp1, 0);
AudioConnection          gainModExpToOffset(linearToExp1, 0, gainModOffset1, 0);
AudioConnection          gainModOffsetToVCA1(gainModOffset1, 0, vca1, 1);
AudioConnection          vcaToOut1(vca1, 0, output1, 0);

AudioConnection          audioToVCA2(audioIn2, 0, vca2, 0);
AudioConnection          gainModToScale2(gainModIn2, 0, gainModScale2, 0);
AudioConnection          gainModScaleToExp2(gainModScale2, 0, linearToExp2, 0);
AudioConnection          gainModExpToOffset2(linearToExp2, 0, gainModOffset2, 0);
AudioConnection          gainModOffsetToVCA2(gainModOffset2, 0, vca2, 1);
AudioConnection          vca2ToMixer2(vca2, 0, mixer2, 0);
AudioConnection          vca1ToMixer2(vca1, 0, mixer2, 1);
AudioConnection          mixerToOut2(mixer2, 0, output2, 0);

AudioConnection          audioToVCA3(audioIn3, 0, vca3, 0);
AudioConnection          gainModToScale3(gainModIn3, 0, gainModScale3, 0);
AudioConnection          gainModScaleToExp3(gainModScale3, 0, linearToExp3, 0);
AudioConnection          gainModExpToOffset3(linearToExp3, 0, gainModOffset3, 0);
AudioConnection          gainModOffsetToVCA3oVCA3(gainModOffset3, 0, vca3, 1);
AudioConnection          vca3ToMixer3(vca3, 0, mixer3, 0);
AudioConnection          vca2ToMixer3(vca2, 0, mixer3, 1);
AudioConnection          mixerToOut3(mixer3, 0, output3, 0);


// Pot and Button Pins

#define MOD_EXP_1_DIGITAL_PIN 2
#define MOD_EXP_1_ANALOG_PIN 17
#define MOD_OFFSET_1_DIGITAL_PIN 3
#define MOD_OFFSET_1_ANALOG_PIN 24
#define MOD_SCALE_1_DIGITAL_PIN 4
#define MOD_SCALE_1_ANALOG_PIN 18

#define MOD_EXP_1_DIGITAL_PIN 2
#define MOD_EXP_1_ANALOG_PIN 17
#define MOD_OFFSET_1_DIGITAL_PIN 3
#define MOD_OFFSET_1_ANALOG_PIN 24
#define MOD_SCALE_1_DIGITAL_PIN 4
#define MOD_SCALE_1_ANALOG_PIN 18

#define MOD_EXP_2_DIGITAL_PIN 6
#define MOD_EXP_2_ANALOG_PIN 19
#define MOD_OFFSET_2_DIGITAL_PIN 5
#define MOD_OFFSET_2_ANALOG_PIN 26
#define MOD_SCALE_2_DIGITAL_PIN 9
#define MOD_SCALE_2_ANALOG_PIN 20

#define MOD_EXP_3_DIGITAL_PIN 10
#define MOD_EXP_3_ANALOG_PIN 22
#define MOD_OFFSET_3_DIGITAL_PIN 11
#define MOD_OFFSET_3_ANALOG_PIN 27
#define MOD_SCALE_3_DIGITAL_PIN 12
#define MOD_SCALE_3_ANALOG_PIN 23

#define VCA_1_OUTPUT_CONNECTED_SWITCH 28
#define VCA_2_OUTPUT_CONNECTED_SWITCH 29


//Pot and Button Objects

Potentiometer modExpPot1(MOD_EXP_1_DIGITAL_PIN, MOD_EXP_1_ANALOG_PIN);
Potentiometer modOffsetPot1(MOD_OFFSET_1_DIGITAL_PIN, MOD_OFFSET_1_ANALOG_PIN);
Potentiometer modScalePot1(MOD_SCALE_1_DIGITAL_PIN, MOD_SCALE_1_ANALOG_PIN);

Potentiometer modExpPot2(MOD_EXP_2_DIGITAL_PIN, MOD_EXP_2_ANALOG_PIN);
Potentiometer modOffsetPot2(MOD_OFFSET_2_DIGITAL_PIN, MOD_OFFSET_2_ANALOG_PIN);
Potentiometer modScalePot2(MOD_SCALE_2_DIGITAL_PIN, MOD_SCALE_2_ANALOG_PIN);

Potentiometer modExpPot3(MOD_EXP_3_DIGITAL_PIN, MOD_EXP_3_ANALOG_PIN);
Potentiometer modOffsetPot3(MOD_OFFSET_3_DIGITAL_PIN, MOD_OFFSET_3_ANALOG_PIN);
Potentiometer modScalePot3(MOD_SCALE_3_DIGITAL_PIN, MOD_SCALE_3_ANALOG_PIN);


  
void setup() {
  AudioMemory(20);
  pinMode(VCA_1_OUTPUT_CONNECTED_SWITCH, INPUT_PULLUP);
  pinMode(VCA_2_OUTPUT_CONNECTED_SWITCH, INPUT_PULLUP);
}

void loop() {
  
  float gainReading = modOffsetPot1.read();
  float expReading = modExpPot1.read();
  float cvGainReading = modScalePot1.read();
  
  AudioNoInterrupts();
  gainModOffset1.offset(gainReading);
  gainModScale1.gain(cvGainReading);
  linearToExp1.setStrength(expReading);
  AudioInterrupts();


  gainReading = modOffsetPot2.read();
  expReading = modExpPot2.read();
  cvGainReading = modScalePot2.read();
  
  AudioNoInterrupts();
  gainModOffset2.offset(gainReading);
  gainModScale2.gain(cvGainReading);
  linearToExp2.setStrength(expReading);
  digitalRead(VCA_1_OUTPUT_CONNECTED_SWITCH) ? vca1ToMixer2.connect() : vca1ToMixer2.disconnect();
  AudioInterrupts();


  gainReading = modOffsetPot3.read();
  expReading = modExpPot3.read();
  cvGainReading = modScalePot3.read();

  AudioNoInterrupts();
  gainModOffset3.offset(gainReading);
  gainModScale3.gain(cvGainReading);
  linearToExp3.setStrength(expReading);
  digitalRead(VCA_2_OUTPUT_CONNECTED_SWITCH) ? vca2ToMixer3.connect() : vca2ToMixer3.disconnect();
  AudioInterrupts();

}
