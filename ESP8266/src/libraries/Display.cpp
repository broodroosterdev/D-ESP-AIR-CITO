#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <bsec.h>
#include "Display.h"
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

Display::Display() {
    Adafruit_SSD1306 _display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);
}
void Display::Begin() {
    if(!_display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    for(;;); // Don't proceed, loop forever
    }
    _display.clearDisplay();
    _display.display();
}
void Display::DrawBitmap(const unsigned char *bitmap) {
    _display.drawBitmap(
        (_display.width()  - 32 ) / 2 + 32,
        (_display.height() - 32) / 2,
        pgm_read_word(*bitmap), 32, 32, 1);
    _display.display();
}
void Display::ShowIAQ(float IAQ){
    _display.clearDisplay();
    DrawBitmap(_gas);
}
void Display::ShowHumidity(float Humidity){
    _display.clearDisplay();
    DrawBitmap(_humidity);
    
}
void Display::ShowTemperature(float Temperature){
    _display.clearDisplay();
    DrawBitmap(_temperature);
}
void Display::StartLoading() {
    _isloading = 1;
    while(_isloading == 1){
        _display.clearDisplay();
        DrawBitmap(_loading);
        delay(500);
        _display.clearDisplay();
        DrawBitmap(_loading1);
        delay(500);
    }
}
void Display::StopLoading() {
    _isloading = 0;
    _display.clearDisplay();
}
void Display::ShowData(float data) {
    String Text = String(data);
    _display.clearDisplay();
    _display.setTextSize(4); // Draw 2X-scale text
    _display.setTextColor(WHITE);
    _display.setCursor(0, 0);
    _display.println(Text);
    _display.display();      // Show initial text
}
void Display::StartSlides(Bsec Sensor) {
    _sliding = 1;
    while(_sliding == 1){
        if(Sensor.iaqAccuracy > 1){
            ShowIAQ(Sensor.iaqEstimate);
        }
        delay(2000);
        ShowTemperature(Sensor.temperature);
        delay(2000);
        ShowHumidity(Sensor.humidity);
        delay(2000);
        
    }
}
void Display::StopSlides() {
    _sliding = 0;
    _display.clearDisplay();
}