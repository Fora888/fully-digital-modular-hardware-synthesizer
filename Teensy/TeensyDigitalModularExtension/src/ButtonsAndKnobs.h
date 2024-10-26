class Potentiometer
{
    public:
        Potentiometer(const uint16_t& digitalPin, const uint16_t& analogPin) 
        { 
            pinMode(digitalPin, OUTPUT); 
            pinMode(analogPin, INPUT_DISABLE); 
            this->digitalPin = digitalPin; 
            this->analogPin = analogPin; 
        }

        float read()
        {
            digitalWrite(digitalPin, HIGH);
            uint sample = 0;
            for (int i = 0; i < oversampling; i++)
            {
                sample += analogRead(analogPin);
            }
            digitalWrite(digitalPin, LOW);

            previousEMASample = previousEMASample + alpha * ((sample / (float)oversampling) - previousEMASample);
            
            if(abs(previousSample - int(previousEMASample)) > 1)
                previousSample = previousEMASample;
                  
            return previousSample / 1023.0f;
        }

    private:
        uint16_t digitalPin, analogPin;
        float alpha = 0.05, previousEMASample = 0;
        int previousSample = 0;
        const uint16_t oversampling = 1;
};

class Button
{
  public:
    Button(const uint16_t& digitalPin, bool inverted){ pinMode(digitalPin, INPUT_PULLUP); this->buttonPin = digitalPin; this->inverted = inverted;}
    Button(const uint16_t& digitalPin, bool inverted, uint8_t mode) { pinMode(digitalPin, mode); this->buttonPin = digitalPin; this->inverted = inverted; }

    bool read()
    {
      bool reading = digitalRead(buttonPin);

      if(debounceTime > debounceDelay)
      {
        lastState = reading;
        debounceTime = 0;
      }
      
      if(reading != lastState)
      {
        debounceTime += millis() - lastTime;
      }
      else
      {
        debounceTime = 0;
      }
      lastTime = millis();
      return inverted ? !lastState : lastState;
    };

  private:
    uint16_t buttonPin;
    bool inverted = false;
  
    bool lastState = false;
    uint32_t debounceTime = 0;
    uint32_t lastTime = 0;

    const uint16_t debounceDelay = 10;
};

class SignalInput
{
    public: 
        SignalInput(const uint16_t& analogPin) { this->analogPin = analogPin; }

        float read()
        {
            uint sample = 0;
            for (int i = 0; i < oversampling; i++)
            {
                sample += analogRead(analogPin);
            }
            return sample / (float)oversampling;
        }

    private:
        uint16_t analogPin;
        const uint16_t oversampling = 256;
};
