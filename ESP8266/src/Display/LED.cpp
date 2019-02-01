#include <Adafruit_NeoPixel.h>
#include "LED.h"


LED::LED(uint8_t pin, uint16_t leds){
  _pin = pin;
  _leds = leds;
  _strip = Adafruit_NeoPixel(_leds, _pin, NEO_GRB + NEO_KHZ800);
}

void LED::Begin()
{
  _strip.begin();
  _strip.show();
  _strip.setBrightness(255);
}

void LED::ShowIAQ(float IAQ)
{
  int iaq = static_cast<int>(round(IAQ));
  int ledscore = map(350 - iaq,350,0,20,0);
  if(ledscore < 0){ledscore = 0;}
  uint32_t displayColor = _strip.Color(_colors[ledscore][0],_colors[ledscore][1],_colors[ledscore][2]);
  _strip.fill(displayColor, 0, _leds);
  _strip.show();
}
