#ifndef DISPLAY1306_H
#define DISPLAY1306_H

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

class Display1306 {
  public:
    int _delays;
  
    Display1306(int delays): _delays(delays), status_disp(0) { }
    bool begin();
    void viewsErrors(String text, int starts, int stops, int sizes);
    void loops(int fs, bool isVideo, uint32_t fs58, bool isVideo58);
  private:
    int status_disp;

    void viewText(String str, int starts, int stops, int sizes);
};

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

bool Display1306::begin() {
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    return false;
  }

  display.clearDisplay();
  viewText("FPV DETECT", 20, 20, 1);
  delay(1000);
  display.display();

  return true;
}

void Display1306::viewsErrors(String text, int starts, int stops, int sizes) {
  display.clearDisplay();
  viewText(text, 15, 30, 1);
  display.display();
}

void Display1306::loops(int fs, bool isVideo, uint32_t fs58, bool isVideo58) {
  display.clearDisplay();

  for (int i = 0; i <= 64; i++){
      viewText(".", 60, i, 1);
  }

  for (int i = 0; i <= 120; i++){
      viewText(".", i, 15, 1);
  }

  viewText(String(fs) + "mhz", 10, 25, 1);
  viewText("1.2G", 5, 0, 2);
  viewText("5.8G", 70, 0, 2); 
  viewText(String(fs58) + "mhz", 70, 25, 1);

  if (isVideo) {
    viewText("FPV", 10, 40, 2);
  }

  if (isVideo58) {
    viewText("FPV", 70, 40, 2); 
  }

  display.display();
}

void Display1306::viewText(String str, int starts, int stops, int sizes) {
  display.setTextSize(sizes); // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(starts,stops); // Start at top-left corner
  display.println(str);
}

#endif
