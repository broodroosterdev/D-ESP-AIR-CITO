#include <Adafruit_NeoPixel.h> //Library van Adafruit voor de neopixel strip(WS2812B)
#include "LED.h"
#include "bsec.h"

LED::LED(uint8_t pin, uint16_t leds){ //Contrstructor voor de LED library
  _pin = pin;
  _leds = leds;
  _strip = Adafruit_NeoPixel(_leds, _pin, NEO_GRB + NEO_KHZ800); //creates new ledstrip object
}
void LED::Begin() //Functie om de library te initialiseren
{
  _strip.begin();
  _strip.show();
  _strip.setBrightness(128);
}
void LED::ShowIAQ(float IAQ) //Functie om de IAQ waardes om te zetten in kleur code van groen naar rood.
{
  if(_enabled == 1){
    if(!(_previous_iaq == IAQ) || _reinit == 1){
      _previous_iaq = IAQ;
      int iaq = static_cast<int>(round(IAQ));
      int ledscore = map(350 - iaq,350,0,20,0);
      if(ledscore < 0){ledscore = 0;}
      uint32_t displayColor = _strip.Color(_colors[ledscore][0],_colors[ledscore][1],_colors[ledscore][2]);
      _strip.fill(displayColor, 0, _leds);
      _strip.show();
      _reinit = 0;
    }
  }
}
void LED::EnableLED(Bsec Sensor){ //Zet de ledstrip aan
  if(_enabled == 0){
    	_enabled = 1;
      _reinit = 1;
      LED::ShowIAQ(Sensor.iaqEstimate);
  }else{
    LED::DisableLED();
  }
}
void LED::DisableLED(){ //Zet de ledstrip uit
  if(_enabled == 1){
    _enabled = 0;
    _strip.fill(_strip.Color(0,0,0), 0, _leds);
    _strip.show();
  }
}
