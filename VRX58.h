#ifndef VRX58_H
#define VRX58_H

#include <SPI.h>

#define BIT_BANG_FREQ 10000
#define SYNTHESIZER_REG_A 0x00
#define SYNTHESIZER_REG_B 0x01
#define SYNTHESIZER_REG_C 0x02
#define SYNTHESIZER_REG_D 0x03
#define VCO_SWITCH_CAP_CONTROL_REGISTER 0x04
#define DFC_CONTROL_REGISTER 0x05
#define SIXM_AUDIO_DEMODULATOR_CONTROL_REGISTER 0x06
#define SIXM5_AUDIO_DEMODULATOR_CONTROL_REGISTER 0x07
#define RECEIVER_CONTROL_REGISTER_1 0x08
#define RECEIVER_CONTROL_REGISTER_2 0x09
#define POWER_DOWN_CONTROL_REGISTER 0x0A
#define STATE_REGISTER 0x0F
#define RX5808_READ_CTRL_BIT 0x00
#define RX5808_WRITE_CTRL_BIT 0x01
#define RX5808_ADDRESS_R_W_LENGTH 5
#define RX5808_DATA_LENGTH 20
#define RX5808_PACKET_LENGTH 25

class VRX58 {
  public:
    bool _isVideo58;

    VRX58(int PIN_MOSI, int PIN_CLK, int PIN_CS, int PIN_RSSI);
  
    bool begin();
    void loops();
    bool isVideo(int delays);
    float calc(int pin,int delays);
    int getRSSI();
    void setChanel(uint8_t index);
    uint32_t getChanel();
    void nextChanel();
  private:
    bool _SPIModeEnabled;
    int _PIN_MOSI;
    int _PIN_CLK;
    int _PIN_CS;
    int _pin;
    int _index_chanel;
    int _PIN_RSSI;
    uint16_t _frequencyTable[56] = {
      5865, 5845, 5825, 5805, 5785, 5765, 5745, 5725, // A
      5733, 5752, 5771, 5790, 5809, 5828, 5847, 5866, // B
      5705, 5685, 5665, 5645, 5885, 5905, 5925, 5945, // E
      5740, 5760, 5780, 5800, 5820, 5840, 5860, 5880, // F
      5658, 5695, 5732, 5769, 5806, 5843, 5880, 5917, // R
      5333, 5373, 5413, 5453, 5493, 5533, 5573, 5613, // L
      4990, 5020, 5050, 5080, 5110, 5140, 5170, 5200  // x
    };
    
    uint32_t rtc6705readRegister(uint8_t readRegister);
    void rtc6705WriteRegister(uint32_t buf);
    void EnableSPIMode();
};

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
  _index_chanel;
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

float VRX58::calc(int pin,int delays) {
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

    if (millis() - times  >= delays) {
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

#endif
