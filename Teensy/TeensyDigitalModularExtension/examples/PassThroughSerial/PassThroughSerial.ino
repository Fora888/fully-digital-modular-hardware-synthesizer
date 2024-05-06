/*
 * A simple hardware test which receives audio UART 1 and 2
 * and sends it to UART 1 and 2 output.
 *
 * This example code is in the public domain.
 */

#include <Audio.h>
#include <TeensyDigitalModularExtension.h>

AudioInputSerial            channel1Input(&Serial1);
AudioInputSerial            channel2Input(&Serial2);

AudioOutputSerial           channel1Output(&Serial1);
AudioOutputSerial           channel2Output(&Serial2);

AudioConnection          	patchCord1(channel1Input, channel1Output);
AudioConnection          	patchCord2(channel2Input, channel2Output);

void setup() {
  // Audio connections require memory to work.  For more
  // detailed information, see the MemoryAndCpuUsage example
  AudioMemory(12);
}

void loop() {
  // Do nothing here.  The Audio flows automatically

  // When AudioInputAnalog is running, analogRead() must NOT be used.
}