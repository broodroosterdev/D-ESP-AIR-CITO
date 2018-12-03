#ifndef LED_h
#define LED_h

#include <Adafruit_NeoPixel.h>

class LED
{
  public:
    LED(uint8_t pin=12, uint16_t leds=5);
    void Begin();
    void ShowIAQ(float IAQ);
  private:
    uint8_t _pin;
    uint16_t _leds;
    uint8_t _colors[21][3] = {
        {255,0,0},
        {242,13,0},
        {230,26,0},
        {217,38,0},
        {204,51,0},
        {191,64,0},
        {178,76,0},
        {166,89,0},
        {153,102,0},
        {140,115,0},
        {128,128,0},
        {115,140,0},
        {102,153,0},
        {89,166,0},
        {77,178,0},
        {64,191,0},
        {51,204,0},
        {38,217,0},
        {25,230,0},
        {13,242,0},
        {0,255,0}
        };
    Adafruit_NeoPixel _strip;
};


#endif
