#include "Display.h"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

Display::Display(){
    Adafruit_SSD1306 _display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);
}
void Display::Begin(){
    if (!_display.begin(SSD1306_SWITCHCAPVCC, 0x3C)){ // Address 0x3C for 128x32
    for(;;); // Don't proceed, loop forever
    }
    _display.clearDisplay();
    _display.display();
}
void Display::DrawBitmap(const uint8_t bitmap[]){
    _display.drawBitmap(
        (_display.width()  - 32 ) / 2 + 32,
        (_display.height() - 32) / 2,
        bitmap, 32, 32, 1);
}
void Display::ShowIAQ(float IAQ){
    _display.clearDisplay();
    DrawBitmap(bit_gas);
}
void Display::ShowHumidity(float Humidity){
    _display.clearDisplay();
    DrawBitmap(bit_gas);
    
}
void Display::ShowTemperature(float Temperature){
    _display.clearDisplay();
    DrawBitmap(bit_temperature);

}
void Display::ShowError(String error){
    ShowText(error);
    DrawBitmap(bit_warning);
    _display.display();
}
void Display::ShowData(float data) {
    String Text = String(data);
    _display.clearDisplay();
    _display.setTextSize(2); // Draw 2X-scale text
    _display.setTextColor(WHITE);
    _display.setCursor(0, 0);
    _display.println(Text);
    _display.display();      // Show initial text
}
void Display::ShowText(String text) {
    _display.clearDisplay();
    _display.setTextSize(2); // Draw 2X-scale text
    _display.setTextColor(WHITE);
    _display.setCursor(0, 0);
    _display.println(text);
    _display.display();      // Show text on display
}
void Display::StartLoading(){
    _isloading = 1;
    _display.clearDisplay();
    DrawBitmap(bit_humidity);
}
void Display::Load(String Text){
    _loadingtext = Text;
    LoadingStep();
}
void Display::LoadingStep (void){
  if(_isloading == 1){
    _loadingstep = (_loadingstep == 1) ? 0 : 1;
  if(_loadingstep == 1){
        ShowText(_loadingtext);
        _display.drawBitmap(
        (_display.width()  - 32 ) / 2 + 32,
        (_display.height() - 32) / 2,
        bit_loading, 32, 32, 1);
        _display.display();
  }else{
        ShowText(_loadingtext);
        _display.drawBitmap(
        (_display.width()  + 32 ) / 2 + 32,
        (_display.height() + 32) / 2,
        bit_loading1, 32, 32, 1);
        _display.display();
    }
  }
}
void Display::StopLoading() {
    _isloading = 0;
    _display.clearDisplay();
}
void Display::StartSlides() {
    _sliding = 1;
}
void Display::StopSlides() {
    _sliding = 0;
    _display.clearDisplay();
}
void Display::NextSlide(Bsec Sensor) {
    if(_slide == 0){
        if(Sensor.iaqAccuracy > 1){
            ShowIAQ(Sensor.iaqEstimate);
            _display.display();
        }
    }else if(_slide == 1){
        ShowTemperature(Sensor.temperature);
        _display.display();
    }else if(_slide == 2){
        ShowHumidity(Sensor.humidity);
        _display.display();
    }
}
