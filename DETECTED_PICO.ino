#include <Wire.h>
#include "TA8804.h"
#include "VRX58.h"
#include "display_1306.h"
display_1306 D1306(100);
TA8804 tuner(28);
VRX58 vrx5808(13,11,12,0);
void setup() {
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
  tuner.loops();
  vrx5808.loops();
  D1306.loops(tuner._fs,tuner._isVideo,vrx5808.getChanel(),vrx5808._isVideo58);
//  Serial.println(isVideo(28,50));
}
