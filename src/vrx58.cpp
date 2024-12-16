#include "vrx58.h"

VRX58::VRX58(int PIN_MOSI, int PIN_CLK, int PIN_CS, int PIN_RSSI) {
  _PIN_MOSI = PIN_MOSI;
  _PIN_CLK = PIN_CLK;
  _PIN_CS = PIN_CS;
  _PIN_RSSI = PIN_RSSI;
  _isVideo58 = false;
  _SPIModeEnabled = false;
  _PIN_MOSI = 23;
  _PIN_CLK = 18;
  _PIN_CS = 32;
  _pin = 26;
  _index_chanel = 0;
  _PIN_RSSI = 0;
}

bool VRX58::begin() {
  pinMode(_PIN_MOSI, INPUT);
  pinMode(_PIN_CLK, INPUT);
  pinMode(_PIN_CS, INPUT);

  EnableSPIMode();
  setChanel(0);

  return rtc6705readRegister(SYNTHESIZER_REG_B) == 0;
}

void VRX58::loops() {
  if(!isVideo(100)) {
    _index_chanel++;

    if(_index_chanel > 55) {
      _index_chanel = 0;
    }

    setChanel(_index_chanel);
  }
}

bool VRX58::isVideo(int delays) {
  int rss = getRSSI();

  if (rss <= 55) {
    return false;
  }

  float dat = calc(_pin, delays);

  return _isVideo58 = dat > 100 && dat < 230;
}

float VRX58::calc(int pin, int delays) {
  unsigned long times = millis();
  int d = 1;
  int newVal = 0;
  float dat = 0;
  int i = 0;

  while (d) {
    newVal = analogRead(pin);

    if (newVal > 350 && newVal < 450) {
      dat += newVal;
    }

    i++;

    if (millis() - times >= (long unsigned) delays) {
      d = 0;
    }
  }

  return dat / i;
}

int VRX58::getRSSI() {
  return map(analogRead(_PIN_RSSI), 0, 360, 0, 100);
}

void VRX58::setChanel(uint8_t index) {
    _index_chanel = index;

    uint16_t f = _frequencyTable[index];
    uint32_t data = ((((f - 479) / 2) / 32) << 7) | (((f - 479) / 2) % 32);
    uint32_t newRegisterData = SYNTHESIZER_REG_B  | (RX5808_WRITE_CTRL_BIT << 4) | (data << 5);
    uint32_t currentRegisterData = SYNTHESIZER_REG_B | (RX5808_WRITE_CTRL_BIT << 4) | rtc6705readRegister(SYNTHESIZER_REG_B);

    if (newRegisterData != currentRegisterData) {
      rtc6705WriteRegister(newRegisterData);
    }
}

uint32_t VRX58::getChanel() {
  return _frequencyTable[_index_chanel];
}

void VRX58::nextChanel() {
  _isVideo58 = false;
  _index_chanel++;
  setChanel(_index_chanel);
}

uint32_t VRX58::rtc6705readRegister(uint8_t readRegister) {
  if (!_SPIModeEnabled) {
    EnableSPIMode();
  }

  uint32_t buf = readRegister | (RX5808_READ_CTRL_BIT << 4);
  uint32_t registerData = 0;
  uint32_t periodMicroSec = 1000000 / BIT_BANG_FREQ;

  digitalWrite(_PIN_CS, LOW);
  delayMicroseconds(periodMicroSec);

  // Write register address and read bit
  for (uint8_t i = 0; i < RX5808_ADDRESS_R_W_LENGTH; ++i) {
      digitalWrite(_PIN_CLK, LOW);
      delayMicroseconds(periodMicroSec / 4);
      digitalWrite(_PIN_MOSI, buf & 0x01);
      delayMicroseconds(periodMicroSec / 4);
      digitalWrite(_PIN_CLK, HIGH);
      delayMicroseconds(periodMicroSec / 2);

      buf >>= 1; 
  }

  // Change pin from output to input
  pinMode(_PIN_MOSI, INPUT);

  // Read data 20 bits
  for (uint8_t i = 0; i < RX5808_DATA_LENGTH; i++) {
      digitalWrite(_PIN_CLK, LOW);
      delayMicroseconds(periodMicroSec / 4);

      if (digitalRead(_PIN_MOSI)) {
        registerData = registerData | (1 << (5 + i));
      }

      delayMicroseconds(periodMicroSec / 4);
      digitalWrite(_PIN_CLK, HIGH);
      delayMicroseconds(periodMicroSec / 2);
  }

  // Change pin back to output
  pinMode(_PIN_MOSI, OUTPUT);

  digitalWrite(_PIN_MOSI, LOW);
  digitalWrite(_PIN_CLK, LOW);
  digitalWrite(_PIN_CS, HIGH);

  return registerData;
}

void VRX58::rtc6705WriteRegister(uint32_t buf) {
  if (!_SPIModeEnabled) {
      EnableSPIMode();
  }

  uint32_t periodMicroSec = 1000000 / BIT_BANG_FREQ;

  digitalWrite(_PIN_CS, LOW);
  delayMicroseconds(periodMicroSec);

  for (uint8_t i = 0; i < RX5808_PACKET_LENGTH; ++i) {
    digitalWrite(_PIN_CLK, LOW);
    delayMicroseconds(periodMicroSec / 4);
    digitalWrite(_PIN_MOSI, buf & 0x01);
    delayMicroseconds(periodMicroSec / 4);
    digitalWrite(_PIN_CLK, HIGH);
    delayMicroseconds(periodMicroSec / 2);

    buf >>= 1; 
  }

  digitalWrite(_PIN_CLK, LOW);
  delayMicroseconds(periodMicroSec);
  digitalWrite(_PIN_MOSI, LOW);
  digitalWrite(_PIN_CLK, LOW);
  digitalWrite(_PIN_CS, HIGH);
}

void VRX58::EnableSPIMode() {
  pinMode(_PIN_MOSI, OUTPUT);
  pinMode(_PIN_CLK, OUTPUT);
  pinMode(_PIN_CS, OUTPUT);

  digitalWrite(_PIN_MOSI, LOW);
  digitalWrite(_PIN_CLK, LOW);
  digitalWrite(_PIN_CS, HIGH);

  _SPIModeEnabled = true;
}

