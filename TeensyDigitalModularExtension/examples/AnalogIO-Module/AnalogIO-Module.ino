#include <Audio.h>
#include <TeensyDigitalModularExtension.h>

AudioInputI2S2                  i2s2_1;
AudioOutputI2S                  i2s1;
AudioInputSerial                leftInput(&Serial1);
AudioInputSerial                rightInput(&Serial3);
AudioOutputSerial               leftOutput(&Serial1);
AudioOutputSerial               rightOutput(&Serial3);

AudioConnection          patchCord1(i2s2_1, 0, leftOutput, 0);
AudioConnection          patchCord2(i2s2_1, 1, rightOutput, 0);
AudioConnection          patchCord3(leftInput, 0, i2s1, 0);
AudioConnection          patchCord4(leftInput, 0, i2s1, 1);
// GUItool: end automatically generated code
void setup() {
  AudioMemory(12);
  pinMode(6, INPUT_PULLUP);
  delay(1);
  attachInterrupt(6, changeConnection, CHANGE);
}

void loop() {
  // put your main code here, to run repeatedly:

}

void changeConnection()
{
  patchCord4.disconnect();
  if(digitalRead(6))
  {
    patchCord4.connect(leftInput, 0, i2s1, 1);
  }
  else
  {
    patchCord4.connect(rightInput, 0, i2s1, 1); 
  }
  delay(10);
}
