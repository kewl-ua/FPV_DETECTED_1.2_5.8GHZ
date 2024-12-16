#include "display1306.h"

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
