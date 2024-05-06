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


//PitchOutput 1

AudioMIDIPitchChangeInput     channel1PitchInput(1);
AudioAmplifier                channel1PitchOutputAmp;
AudioOutputSerial             channel1PitchCVOutput(&Serial7);
AudioMIDIHostPitchChangeInput midiHostPitchInput(1);
AudioForwardLastChanged       pitchSignalForwarder;

AudioConnection               pitch1ToForwarder(channel1PitchInput, 0, pitchSignalForwarder, 0);
AudioConnection               pitchHostToForwarder(midiHostPitchInput, 0, pitchSignalForwarder, 1);
AudioConnection               pitchForwarderToAmp(pitchSignalForwarder, 0, channel1PitchOutputAmp, 0);
AudioConnection               amp1ToOut(channel1PitchOutputAmp, 0, channel1PitchCVOutput, 0);


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


//PitchOutput 2

AudioMIDIPitchChangeInput     channel2PitchInput(2);
AudioAmplifier                channel2PitchOutputAmp;
AudioOutputSerial             channel2PitchCVOutput(&Serial6);
AudioMIDIHostControlInput     usbHostControlInput(1,1);
AudioForwardLastChanged       controlSignalForwarder;

AudioConnection               pitch2ToForwarder(channel2PitchInput, 0, controlSignalForwarder, 0);
AudioConnection               controlHostToForwarder(usbHostControlInput, 0, controlSignalForwarder, 1);
AudioConnection               controlForwarderToAmp(controlSignalForwarder, 0, channel2PitchOutputAmp, 0);
AudioConnection               amp2ToOut(channel2PitchOutputAmp, 0, channel2PitchCVOutput, 0);


//NoteInput 1

SC16IS7xxInput                channel1TriggerIn(&sc16is7Manager, 1);
AudioInputSerial              channel1NoteIn(&Serial4);
AudioMIDINoteOutput           channel1Output(1);
AudioLED                      channel1TriggerInIndicator(4);

AudioConnection               triggerInput1ToMIDI(channel1TriggerIn, 0, channel1Output, 1);
AudioConnection               noteInput1ToMIDI(channel1NoteIn, 0, channel1Output, 0);
AudioConnection               triggerInput1ToLED(channel1TriggerIn, 0, channel1TriggerInIndicator, 0);


//PitchInput1

AudioInputSerial              channel1PitchCVInput(&Serial7);
AudioAmplifier                channel1PitchInputAmp;
AudioMIDIPitchChangeOutput    channel1PitchOutput(1);

AudioConnection               pitch1CVToAmp(channel1PitchCVInput, 0, channel1PitchInputAmp, 0);
AudioConnection               amp1ToMIDIOut(channel1PitchInputAmp, 0, channel1PitchOutput, 0);


//NoteInput 2

AudioInputSerial              channel2TriggerIn(&Serial3);
AudioInputSerial              channel2NoteIn(&Serial5);
AudioMIDINoteOutput           channel2Output(2);
AudioLED                      channel2TriggerInIndicator(5);

AudioConnection               triggerInput2ToMIDI(channel2TriggerIn, 0, channel2Output, 1);
AudioConnection               noteInput2ToMIDI(channel2NoteIn, 0, channel2Output, 0);
AudioConnection               triggerInput2ToLED(channel2TriggerIn, 0, channel2TriggerInIndicator, 0);


//PitchInput2

AudioInputSerial              channel2PitchCVInput(&Serial6);
AudioAmplifier                channel2PitchInputAmp;
AudioMIDIPitchChangeOutput    channel2PitchOutput(2);

AudioConnection               pitch2CVToAmp(channel2PitchCVInput, 0, channel2PitchInputAmp, 0);
AudioConnection               amp2ToMIDIOut(channel2PitchInputAmp, 0, channel2PitchOutput, 0);


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

#define CHANNEL_1_PITCH_OUTPUT_GAIN_DIGITAL_PIN 30
#define CHANNEL_1_PITCH_OUTPUT_GAIN_ANALOG_PIN 19

#define CHANNEL_2_PITCH_OUTPUT_GAIN_DIGITAL_PIN 31
#define CHANNEL_2_PITCH_OUTPUT_GAIN_ANALOG_PIN 18

#define CHANNEL_1_PITCH_INPUT_GAIN_DIGITAL_PIN 32
#define CHANNEL_1_PITCH_INPUT_GAIN_ANALOG_PIN 23

#define CHANNEL_2_PITCH_INPUT_GAIN_DIGITAL_PIN 33
#define CHANNEL_2_PITCH_INPUT_GAIN_ANALOG_PIN 22


//Pot objects

Potentiometer pitchOut1Gain(CHANNEL_1_PITCH_OUTPUT_GAIN_DIGITAL_PIN,CHANNEL_1_PITCH_OUTPUT_GAIN_ANALOG_PIN);
Potentiometer pitchOut2Gain(CHANNEL_2_PITCH_OUTPUT_GAIN_DIGITAL_PIN,CHANNEL_2_PITCH_OUTPUT_GAIN_ANALOG_PIN);

Potentiometer pitchIn1Gain(CHANNEL_1_PITCH_INPUT_GAIN_DIGITAL_PIN,CHANNEL_1_PITCH_INPUT_GAIN_ANALOG_PIN);
Potentiometer pitchIn2Gain(CHANNEL_2_PITCH_INPUT_GAIN_DIGITAL_PIN,CHANNEL_2_PITCH_INPUT_GAIN_ANALOG_PIN);



void setup() 
{
  AudioMemory(30);
  // put your setup code here, to run once:

}

void loop() 
{

  //Read Pots
  
  float pitchOut1GainValue = pitchOut1Gain.read();
  float pitchOut2GainValue = pitchOut2Gain.read();

  float pitchIn1GainValue = pitchIn1Gain.read();
  float pitchIn2GainValue = pitchIn2Gain.read();


  //Apply new values 
  
  AudioNoInterrupts();
  channel1PitchOutputAmp.gain(pitchOut1GainValue);
  channel2PitchOutputAmp.gain(pitchOut2GainValue);

  channel1PitchInputAmp.gain(pitchIn1GainValue);
  channel2PitchInputAmp.gain(pitchIn2GainValue);
  AudioInterrupts();
}
