#include <Wire.h>
#include "TA8804.h"
#include "VRX58.h"
#include "display_1306.h"
display_1306 D1306(100);
TA8804 tuner(28);
VRX58 vrx5808(13,11,12,27);
void setup() {
  pinMode(10,OUTPUT);
  pinMode(23,INPUT);
  digitalWrite(10, LOW);
  Wire.begin();
  if(D1306.begin()){
    //если ошибка Дисплея
      if(tuner.begin()){
      //если ошибка тюнера
        D1306.viewsErrors("Error RX 8804",15,30,1);
        for(;;); // Don't proceed, loop forever
      }
      if(!vrx5808.begin()){
        D1306.viewsErrors("Error RX 5808",15,30,1);
        for(;;); // Don't proceed, loop forever
      }
  }
  // put your setup code here, to run once:
  Serial.begin(115200);
  
}
void loop() {
  int key = digitalRead(23);
  if(key >= 1){
    vrx5808.nextChanel();
  }
  Serial.println(key);
  tuner.loops();
  vrx5808.loops();
  D1306.loops(tuner._fs,tuner._isVideo,vrx5808.getChanel(),vrx5808._isVideo58);

  if(vrx5808._isVideo58 || tuner._isVideo){
    digitalWrite(10, HIGH);
  }else{
    digitalWrite(10, LOW);
  }
//  Serial.println(isVideo(28,50));
}
