# Fully digital modular synthesizer
A fully digital modular synthesizer specification based on eurorack

## Spezification

### Audio data format
The audio data is transmitted and processed at 44.1kHz sample rate and 16 Bits of resolution

### Data transmission
Data is transmitted with a simple protocoll over UART.
UART is configured at 1,500,00 baud with 8 data bits, 1 stop bit, no flow control and regular polarity. Singal levels are 0V low and 3.3V high. All inputs and outputs should however be able to tolerate +12V to -12V to avoid accidental damage in scenarios where analog equipment is also used.
Since each audio sample requires 16 bits and only 8 bits can be transmissioned via UART at a time, each sample is sent as three sequential packets. Each packet consists of 6 data bits followed by 2 order bits. Therefore a full UART frame looks like the following:
|1|6|2|1|
|-|-|-|-|
|Start bit|Data bits|Order bits|Stop bit|

The least significant 8 bits of a sample are sent first with order bits 00, followed by the middle 8 bits with order bits 10 and the most significant 4 bits with order bits 01. The last two data bits of the last packet with order bits 01 aswell as the order bit combination 11 are currently unused, but can serve for extensions in the future.

### Power
Since microcontrollers only require 5V, a power specification is planed. Currently however eurorack powersupplies in combination with 5V regulators are used

### Mechanical
Since UART only requires one cable for transmission, similiar to mono audio, the mechanical specifications of virtually any existing analog modular synthesizer can be used. The examples are based on eurorack modules with 128mm height and 3.5mm TS (mono) phone connectors.

## Why digital?

