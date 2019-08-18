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
void LED::ShowRainbow(){
  uint16_t i, j;
  for(j=0; j<256; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< _leds; i++) {
      _strip.setPixelColor(i, LED::Wheel((i * 256 / _leds + j) & 255));
    }
    _strip.show();
    delay(10);
}
}
uint32_t LED::Wheel(byte WheelPos){
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return _strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return _strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return _strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
