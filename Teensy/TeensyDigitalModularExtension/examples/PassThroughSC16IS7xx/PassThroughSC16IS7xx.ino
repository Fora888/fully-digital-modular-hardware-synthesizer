/*
 * A simple hardware test which receives audio on UART 1 of the SC16IS7xx SPI to UART bridge
 * and sends it to UART 1 of the SC16IS7xx.
 *
 * This example code is in the public domain.
 */

#include <Audio.h>
#include <TeensyDigitalModularExtension.h>

SC16IS7xxManager            sc16is7Manager(10);  //Initiate sc16is7xx chip with CS pin 10

SC16IS7xxInput              channel1Input(&sc16is7Manager, 0);

SC16IS7xxOutput             channel1Output(&sc16is7Manager, 0);

AudioConnection          	patchCord1(channel1Input, channel1Output);

void setup() {
  // Audio connections require memory to work.  For more
  // detailed information, see the MemoryAndCpuUsage example
  AudioMemory(12);
}

void loop() {
  // Do nothing here.  The Audio flows automatically

  // When AudioInputAnalog is running, analogRead() must NOT be used.
}