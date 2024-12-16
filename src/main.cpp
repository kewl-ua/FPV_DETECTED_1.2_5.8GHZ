#include <Wire.h>

#include "ta8804.h"
#include "vrx58.h"
#include "display1306.h"

#define INPUT_PIN 23
#define OUTPUT_PIN 10
#define BAUD_RATE 115200

Display1306 D1306(100);
TA8804 tuner(28);
VRX58 vrx5808(13, 11, 12, 27);

void setup() {
  pinMode(OUTPUT_PIN, OUTPUT);
  pinMode(INPUT_PIN, INPUT);

  digitalWrite(OUTPUT_PIN, LOW);

  Wire.begin();

  if (D1306.begin()) {
    //если ошибка Дисплея
      if (tuner.begin()) {
      //если ошибка тюнера
        D1306.viewsErrors("Error RX 8804", 15, 30, 1);
        for (;;); // Don't proceed, loop forever
      }

      if (!vrx5808.begin()) {
        D1306.viewsErrors("Error RX 5808", 15, 30, 1);
        for (;;); // Don't proceed, loop forever
      }
  }

  // put your setup code here, to run once:
  Serial.begin(BAUD_RATE);
}

void loop() {
  int key = digitalRead(INPUT_PIN);

  if (key >= 1) {
    vrx5808.nextChanel();
  }

  Serial.println(key);

  tuner.loops();
  vrx5808.loops();
  D1306.loops(tuner._fs, tuner._isVideo, vrx5808.getChanel(), vrx5808._isVideo58);

  if (vrx5808._isVideo58 || tuner._isVideo) {
    digitalWrite(OUTPUT_PIN, HIGH);
  } else {
    digitalWrite(OUTPUT_PIN, LOW);
  }
  //  Serial.println(isVideo(28,50));
}
