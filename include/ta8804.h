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
    int getFrequency();
    void loops();
  private:
    unsigned int _div;
    byte _divH;
    byte _divL;
    int _pin;
    int _status;
    byte _addr;

    bool setFrequency(uint16_t f);
    float calc(int pin, int delays);
};

#endif