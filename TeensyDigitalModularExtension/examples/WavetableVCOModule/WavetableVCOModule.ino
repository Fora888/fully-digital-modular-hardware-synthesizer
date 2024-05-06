#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include <Adafruit_GFX.h>
#include <TeensyDigitalModularExtension.h>
#include <ButtonsAndKnobs.h>
#include <Adafruit_SSD1306.h>


// Audio Objects

AudioSynthWavetableEurorack     wavetable;
AudioOutputSerial               audioOutput(&Serial1);
AudioInputSerial                voctModulationInput(&Serial3);  

AudioInputSerial                positionModulationInput(&Serial1);
AudioAmplifier                  positionModulationScale;
AudioOffset                     positionModulationOffset;

AudioInputSerial                warpModulationInput(&Serial4);
AudioAmplifier                  warpModulationScale;
AudioOffset                     warpModulationOffset;

AudioInputSerial                fmInput(&Serial2);  
AudioAmplifier                  fmSignalScale;


//Audio Routing

AudioConnection          wavetableToOutput(wavetable, 0, audioOutput, 0);
AudioConnection          voctInputToWavetable(voctModulationInput, 0, wavetable, 1);

AudioConnection          positionModulationInputToScale(positionModulationInput, 0, positionModulationScale, 0);
AudioConnection          positionModulationScaleToOffset(positionModulationScale, 0, positionModulationOffset, 0);
AudioConnection          positionModulationOffsetToWavetable(positionModulationOffset, 0, wavetable, 3);

AudioConnection          warpModulationInputToScale(warpModulationInput, 0, warpModulationScale, 0);
AudioConnection          warpModulationScaleToOffset(warpModulationScale, 0, warpModulationOffset, 0);
AudioConnection          warpModulationOffsetToWavetable(warpModulationOffset, 0, wavetable, 2);

AudioConnection          fmInputToScale(fmInput, 0, fmSignalScale, 0);
AudioConnection          fmScaleToWavetable(fmSignalScale, 0, wavetable, 0);


//Display initialization

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_DC     31
#define OLED_CS     33
#define OLED_RESET  39
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT,
  &SPI, OLED_DC, OLED_RESET, OLED_CS);

float displaySamples[128];


// Pot and Button Pins

#define WARP_OFFSET_DIGITAL_PIN 4
#define WARP_OFFSET_ANALOG_PIN 20
#define WARP_SCALE_DIGITAL_PIN  9
#define WARP_SCALE_ANALOG_PIN  23

#define POSITION_OFFSET_DIGITAL_PIN 2
#define POSITION_OFFSET_ANALOG_PIN 18
#define POSITION_SCALE_DIGITAL_PIN  5
#define POSITION_SCALE_ANALOG_PIN  21

#define FREQUENCY_OFFSET_DIGITAL_PIN  3
#define FREQUENCY_OFFSET_ANALOG_PIN   19

#define FM_SCALE_DIGITAL_PIN  6
#define FM_SCALE_ANALOG_PIN  22

#define NEXT_FILE_BUTTON_PIN  32


//Pot and Button Objects

Potentiometer positionOffsetPot(POSITION_OFFSET_DIGITAL_PIN, POSITION_OFFSET_ANALOG_PIN);
Potentiometer positionScalePot(POSITION_SCALE_DIGITAL_PIN, POSITION_SCALE_ANALOG_PIN);

Potentiometer warpOffsetPot(WARP_OFFSET_DIGITAL_PIN, WARP_OFFSET_ANALOG_PIN);
Potentiometer warpScalePot(WARP_SCALE_DIGITAL_PIN, WARP_SCALE_ANALOG_PIN);

Potentiometer frequencyOffsetPot(FREQUENCY_OFFSET_DIGITAL_PIN, FREQUENCY_OFFSET_ANALOG_PIN);

Potentiometer fmScalePot(FM_SCALE_DIGITAL_PIN, FM_SCALE_ANALOG_PIN);

Button nextFileButton(NEXT_FILE_BUTTON_PIN, false);



void setup() 
{
  wavetable.begin();
  wavetable.setFmRange(500.0f);
  AudioMemory(12);
  display.begin(SSD1306_SWITCHCAPVCC);
  display.setRotation(2);
}

void loop() 
{
  drawDisplay();
  updateInputs();
}

void drawDisplay()
{
  display.clearDisplay();
  wavetable.getWaveform(wavetable.position(), displaySamples, SCREEN_WIDTH);
  for(int i = 0; i<SCREEN_WIDTH - 1; i++)
  {
      display.drawLine(i, (displaySamples[i] * 0.5 + 0.5) * (SCREEN_HEIGHT - 1), i + 1, (displaySamples[i + 1] * 0.5 + 0.5) * (SCREEN_HEIGHT - 1) , SSD1306_WHITE);
    //display.drawPixel(i, (displaySamples[i] * 0.5 + 0.5) * (SCREEN_HEIGHT - 1), SSD1306_WHITE);
  }
  display.display();
}

bool switchedFiles = false;

void updateInputs()
{  
  //Read Pots
  
  float octaveReading = frequencyOffsetPot.read();
  
  float positionScaleReading = positionScalePot.read();
  float positionOffsetReading = positionOffsetPot.read();
  
  float warpScaleReading = warpScalePot.read();
  float warpOffsetReading = -(warpOffsetPot.read() * 2 - 1);
  if(warpOffsetReading < 0.01f && warpOffsetReading > -0.01f) warpOffsetReading = 0.0f;
  
  float fmScaleReading = fmScalePot.read();

  
  //Apply new values
  
  AudioNoInterrupts();
  
  wavetable.setOctave(octaveReading - 3);
  
  positionModulationScale.gain(positionScaleReading);
  positionModulationOffset.offset(positionOffsetReading);
  
  warpModulationScale.gain(warpScaleReading * 2);
  warpModulationOffset.offset(warpOffsetReading);
  
  fmSignalScale.gain(fmScaleReading);
  
  if(nextFileButton.read())
  {
    if(!switchedFiles)
      wavetable.loadNextFile();
    switchedFiles = true;
  }
  else
  {
    switchedFiles = false;
  }
  
  AudioInterrupts();
}
