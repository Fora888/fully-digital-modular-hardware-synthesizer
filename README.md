# Fully digital modular synthesizer
A fully digital modular synthesizer specification

## Spezification

### Audio data format
The audio data is transmitted and processed at 44.1kHz sample rate and 16 Bits of resolution

### Data transmission
Data transmission utilizes UART in combination with a simple protocol.
UART is configured at a baud rate of 2,000,000 with 8 data bits, 1 stop bit, no flow control and regular polarity. Signal levels are defined as 0V for low and 3.3V for high. However, all inputs and outputs should withstand voltages from +12V to -12V to prevent accidental damage when interfaced with analog equipment.
Due to UART's limitation of transmitting only 8 bits at a time and each audio sample requiring 16 bits, each sample is transmitted as three sequential packets. Each packet consists of 6 data bits followed by 2 order bits. Therefore a full UART frame looks like the following:
|1|6|2|1|
|-|-|-|-|
|Start bit|Data bits|Order bits|Stop bit|

The least significant 8 bits of a sample are sent first with order bits '00', followed by the middle 8 bits with order bits '10', and finally the most significant 4 bits with order bits '01'. The last two data bits of the last packet with order bits '01', as well as the order bit combination '11', remain unused for now but could be utilized for future extensions.

### Power
While a power specification is planned, current implementation utilizes eurorack power supplies in conjunction with 5V regulators

### Mechanical
Since UART necessitates only one cable for transmission, akin to mono audio, the mechanical specifications of virtually any existing analog modular synthesizer can be used. The examples are based on eurorack the eurorack specification with 128mm height and 3.5mm TS (mono) phone connectors.

## Comparison to analog

### Advantages
**Less noise** - Digital transmission ensures that the audio signal is received exactly as it was sent, without introducing noise during transmission. Furthermore many analog modules use analog-to-digital and digital-to-analog conversion, which introduce noise into the signal

**Reduced complexity** - Digital signals don't require clean power or buffering in order to deliver good audio. Merely two diodes and one or two resistors, which are always in the exact same configuration, are required per input or output. In addition all I/O is full 16 bit and the audio is inherently DC coupled.

**Lower Price** - Microprocessors are cheap and require relativly few and simple auxiliary components when compared to analog electronics. The reduced complexity of digital signal processing also leads to lower development costs.

### Disadvantages
**Latency** - Digital signal processing always comes with latency. In case of the examples, where audio is processed in blocks of 128 samples, this latency comes in at ~3 ms per module. Fast processors can however reduce the block size, reducing the latency to half of that or even less.

**Module selection** - The digital protocol described is still in its early stages of development and therefore there are no manufacturers for it yet. So the only way to use it is to build your own modules

## Implementations and examples
An implementation for the teensy audio library as well as several example modules can be found in the [Teensy](https://github.com/Fora888/fully-digital-modular-hardware-synthesizer/tree/main/Teensy) folder.
