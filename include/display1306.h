#ifndef DISPLAY1306_H
#define DISPLAY1306_H

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C // See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

extern Adafruit_SSD1306 display;

class Display1306 {
  public:
    int _delays;
  
    Display1306(int delays): _delays(delays) { }

    bool begin();
    void viewsErrors(String text, int starts, int stops, int sizes);
    void loops(int fs, bool isVideo, uint32_t fs58, bool isVideo58);
  private:
    void viewText(String str, int starts, int stops, int sizes);
};

#endif
