#ifndef DISPLAY_h
#define DISPLAY_h

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <bsec.h>
#include <pgmspace.h>
#include "Bitmap.hpp"
class Display
{
  public:
  Display();
  void Begin();
  void DrawBitmap(const uint8_t bitmap[]);
  void ShowIAQ(float IAQ);
  void ShowHumidity(float Humidity);
  void ShowTemperature(float Temperature);
  void ShowError(String error);
  void ShowData(float data);
  void ShowText(String text);
  void StartLoading();
  void Load(String text);
  void LoadingStep();
  void StopLoading();
  void StartSlides();
  void StopSlides();
  void NextSlide(Bsec Sensor);

  private:
  Adafruit_SSD1306 _display;
  int _isloading = 0;
  volatile int _loadingstep = 0;
  int _slide = 0;
  int _sliding = 0;
  String _loadingtext = "";
  Bsec *_sensor;
  
};

#endif
