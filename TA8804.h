#ifndef TA8804_H
#define TA8804_H

#include <Arduino.h>
#include <Wire.h>

class TA8804 {
  public:
    bool _isVideo;
    int _fs;

    TA8804(int pin);

    bool begin();
    bool isVideo(int delays);
    int GetFrequency();
    void loops();
  private:
    unsigned int _Delitel;
    byte _DelitelH;
    byte _DelitelL;
    int _pin;
    int _status;
    byte _addr;

    bool SetFrequency(uint16_t f);
    float calc(int pin, int delays);
};

TA8804::TA8804(int pin) {
  _isVideo = false;
  _fs = 1000;
  _pin = pin; 
  _status = 0;
  _addr = 0x60;
}

bool TA8804::begin() {
  return SetFrequency(_fs);
}

bool TA8804::isVideo(int delays) {
  float dat = calc(_pin, delays);

  return dat > 165;
}

int TA8804::GetFrequency() {
  return _fs;
}

void TA8804::loops() {
  if (_fs > 1500) {
      _fs = 1000; 
  }

  if (!isVideo(50)) {
      _fs += 10;
      SetFrequency(_fs);
      _isVideo = false;
  } else {
    _isVideo = true;
  }
}

bool TA8804::SetFrequency(uint16_t f) {
  _Delitel = f * 8 + 3836; //считаем делитель
  _DelitelH = _Delitel >> 8;
  _DelitelL = _Delitel & 0XFF;

  Wire.beginTransmission(_addr);   // стучимся к синтезатору и передаем байты параметров
  Wire.write(_DelitelH);
  Wire.write(_DelitelL);
  Wire.write(0xCE);
  Wire.write(0x00);

  bool error = Wire.endTransmission();

  return !!error;
}

float TA8804::calc(int pin, int delays) {
  unsigned long times = millis();
  int d = 1;
  int newVal = 0;
  float dat = 0;

  while (d){
    newVal = analogRead(pin);

    if (newVal > 300 && newVal < 600) {
      dat += 0.20;
    }

    if(millis() - times  >= delays) {
      d = 0;
    }
  }

  return dat / (delays / 50);
}

#endif