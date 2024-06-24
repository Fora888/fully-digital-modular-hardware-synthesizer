#include <Audio.h>
#include <ButtonsAndKnobs.h>
#include <TeensyDigitalModularExtension.h>
#include <AudioMIDI.h>

//Initialize SC16IS7xx on pin 10

SC16IS7xxManager              sc16is7Manager(10);      


//NoteOutput 1

AudioMIDINoteInput            channel1Input(1);
AudioOutputSerial             channel1NoteOut(&Serial4);
AudioOutputSerial             channel1TriggerOut(&Serial3);
AudioLED                      channel1TriggerOutIndicator(6);
AudioMIDIHostNoteInput        midiHostInput(1);
AudioForwardLastChanged       noteSignalForwarder;
AudioMixer4                   triggerSignalMixer;

AudioConnection               note1ToNoteForwarder(channel1Input, 0, noteSignalForwarder, 0);
AudioConnection               midiHostNoteToNoteForwarder(midiHostInput, 0, noteSignalForwarder, 1);
AudioConnection               noteForwarderMixerToOutput(noteSignalForwarder, 0, channel1NoteOut, 0);

AudioConnection               midiHostTriggerToMixer(midiHostInput, 1, triggerSignalMixer, 0);
AudioConnection               trigger1ToMixer(channel1Input, 1, triggerSignalMixer, 1);
AudioConnection               triggerMixerToIndicator(triggerSignalMixer, 0, channel1TriggerOutIndicator, 0);
AudioConnection               triggerMixerToOutput(triggerSignalMixer, 0, channel1TriggerOut, 0);


//ModOutput 1

AudioMIDIPitchBendInput     channel1ModInput(1);
AudioAmplifier                channel1ModOutputAmp;
AudioOutputSerial             channel1ModCVOutput(&Serial7);
AudioMIDIHostPitchBendInput midiHostModInput(1);
AudioForwardLastChanged       modSignalForwarder;

AudioConnection               mod1ToForwarder(channel1ModInput, 0, modSignalForwarder, 0);
AudioConnection               modHostToForwarder(midiHostModInput, 0, modSignalForwarder, 1);
AudioConnection               modForwarderToAmp(modSignalForwarder, 0, channel1ModOutputAmp, 0);
AudioConnection               amp1ToOut(channel1ModOutputAmp, 0, channel1ModCVOutput, 0);


//NoteOutput 2

AudioMIDINoteInput            channel2Input(2);
AudioOutputSerial             channel2NoteOut(&Serial5);
SC16IS7xxOutput               channel2TriggerOut(&sc16is7Manager, 1);
AudioLED                      channel2TriggerOutIndicator(9);
AudioForwardLastChanged       noteVelocityForwarder;

AudioConnection               midiHostVelocityToForwarder(channel2Input, 0, noteVelocityForwarder, 0);
AudioConnection               note2ToForwarder(midiHostInput, 2, noteVelocityForwarder, 1);
AudioConnection               forwarder2ToOutput(noteVelocityForwarder, 0, channel2NoteOut, 0);

AudioConnection               trigger2ToIndicator(channel2Input, 1, channel2TriggerOutIndicator, 0);
AudioConnection               trigger2ToOutput(channel2Input, 1, channel2TriggerOutIndicator, 0);


//ModOutput 2

AudioMIDIPitchBendInput     channel2ModInput(2);
AudioAmplifier                channel2ModOutputAmp;
AudioOutputSerial             channel2ModCVOutput(&Serial6);
AudioMIDIHostControlInput     usbHostControlInput(1,1);
AudioForwardLastChanged       controlSignalForwarder;

AudioConnection               mod2ToForwarder(channel2ModInput, 0, controlSignalForwarder, 0);
AudioConnection               controlHostToForwarder(usbHostControlInput, 0, controlSignalForwarder, 1);
AudioConnection               controlForwarderToAmp(controlSignalForwarder, 0, channel2ModOutputAmp, 0);
AudioConnection               amp2ToOut(channel2ModOutputAmp, 0, channel2ModCVOutput, 0);


//NoteInput 1

SC16IS7xxInput                channel1TriggerIn(&sc16is7Manager, 1);
AudioInputSerial              channel1NoteIn(&Serial4);
AudioMIDINoteOutput           channel1Output(1);
AudioLED                      channel1TriggerInIndicator(4);

AudioConnection               triggerInput1ToMIDI(channel1TriggerIn, 0, channel1Output, 1);
AudioConnection               noteInput1ToMIDI(channel1NoteIn, 0, channel1Output, 0);
AudioConnection               triggerInput1ToLED(channel1TriggerIn, 0, channel1TriggerInIndicator, 0);


//ModInput1

AudioInputSerial              channel1ModCVInput(&Serial7);
AudioAmplifier                channel1ModInputAmp;
AudioMIDIPitchBendOutput    channel1ModOutput(1);

AudioConnection               mod1CVToAmp(channel1ModCVInput, 0, channel1ModInputAmp, 0);
AudioConnection               amp1ToMIDIOut(channel1ModInputAmp, 0, channel1ModOutput, 0);


//NoteInput 2

AudioInputSerial              channel2TriggerIn(&Serial3);
AudioInputSerial              channel2NoteIn(&Serial5);
AudioMIDINoteOutput           channel2Output(2);
AudioLED                      channel2TriggerInIndicator(5);

AudioConnection               triggerInput2ToMIDI(channel2TriggerIn, 0, channel2Output, 1);
AudioConnection               noteInput2ToMIDI(channel2NoteIn, 0, channel2Output, 0);
AudioConnection               triggerInput2ToLED(channel2TriggerIn, 0, channel2TriggerInIndicator, 0);


//ModInput2

AudioInputSerial              channel2ModCVInput(&Serial6);
AudioAmplifier                channel2ModInputAmp;
AudioMIDIPitchBendOutput    channel2ModOutput(2);

AudioConnection               mod2CVToAmp(channel2ModCVInput, 0, channel2ModInputAmp, 0);
AudioConnection               amp2ToMIDIOut(channel2ModInputAmp, 0, channel2ModOutput, 0);


//Clock Output

SC16IS7xxOutput               clockOut(&sc16is7Manager, 0);
AudioLED                      clockIndicator(2);
AudioMIDIClockInput           clockInput;

AudioConnection               clockToLED(clockInput, 0, clockIndicator, 0);
AudioConnection               clockToOutput(clockInput, 0, clockOut, 0);


//USB Output

AudioInputSerial              audioInputLeft(&Serial1);
AudioInputSerial              audioInputRight(&Serial2);
AudioOutputUSB                usbOutput;

AudioConnection               audioInLeftToUsb(audioInputLeft, 0, usbOutput, 0);
AudioConnection               audioInRightToUsb(audioInputRight, 0, usbOutput, 1);


//USB Input

AudioOutputSerial             audioOutputLeft(&Serial1);
AudioOutputSerial             audioOutputRight(&Serial2);
AudioInputUSB                 usbInput;

AudioConnection               audioOutLeftToUsb(usbInput, 0, audioOutputLeft, 0);
AudioConnection               audioOutRightToUsb(usbInput, 1, audioOutputRight, 0);


//Pot Pins

#define CHANNEL_1_MOD_OUTPUT_GAIN_DIGITAL_PIN 30
#define CHANNEL_1_MOD_OUTPUT_GAIN_ANALOG_PIN 19

#define CHANNEL_2_MOD_OUTPUT_GAIN_DIGITAL_PIN 31
#define CHANNEL_2_MOD_OUTPUT_GAIN_ANALOG_PIN 18

#define CHANNEL_1_MOD_INPUT_GAIN_DIGITAL_PIN 32
#define CHANNEL_1_MOD_INPUT_GAIN_ANALOG_PIN 23

#define CHANNEL_2_MOD_INPUT_GAIN_DIGITAL_PIN 33
#define CHANNEL_2_MOD_INPUT_GAIN_ANALOG_PIN 22


//Pot objects

Potentiometer modOut1Gain(CHANNEL_1_MOD_OUTPUT_GAIN_DIGITAL_PIN,CHANNEL_1_MOD_OUTPUT_GAIN_ANALOG_PIN);
Potentiometer modOut2Gain(CHANNEL_2_MOD_OUTPUT_GAIN_DIGITAL_PIN,CHANNEL_2_MOD_OUTPUT_GAIN_ANALOG_PIN);

Potentiometer modIn1Gain(CHANNEL_1_MOD_INPUT_GAIN_DIGITAL_PIN,CHANNEL_1_MOD_INPUT_GAIN_ANALOG_PIN);
Potentiometer modIn2Gain(CHANNEL_2_MOD_INPUT_GAIN_DIGITAL_PIN,CHANNEL_2_MOD_INPUT_GAIN_ANALOG_PIN);



void setup() 
{
  AudioMemory(30);
  // put your setup code here, to run once:

}

void loop() 
{

  //Read Pots
  
  float modOut1GainValue = modOut1Gain.read();
  float modOut2GainValue = modOut2Gain.read();

  float modIn1GainValue = modIn1Gain.read();
  float modIn2GainValue = modIn2Gain.read();


  //Apply new values 
  
  AudioNoInterrupts();
  channel1ModOutputAmp.gain(modOut1GainValue);
  channel2ModOutputAmp.gain(modOut2GainValue);

  channel1ModInputAmp.gain(modIn1GainValue);
  channel2ModInputAmp.gain(modIn2GainValue);
  AudioInterrupts();
}
