#include <Adafruit_NeoPixel.h>

#define PIN            12
#define NUMPIXELS      5

uint32_t color;
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

int COLORS[22][3] = {
    {255, 0, 0},
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

void setup() {
    strip.begin();
    strip.show();
    strip.setBrightness(255);
    Serial.begin(115200);
}

void loop() {
    
    for(size_t counter = 0; counter < 21; counter++)
    {  
        
        color = strip.Color(COLORS[counter][0],COLORS[counter][1],COLORS[counter][2]);
        Serial.println(color);
        strip.fill(color, 0, NUMPIXELS);
        strip.show();
        delay(100);
        
    }
    
}