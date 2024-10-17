
class TA8804 {
  public:
    bool _isVideo = false;
    int _fs = 1000;
    TA8804(int pin){
      _pin = pin; 
    }
    bool begin(){
        return SetFrequency(_fs);
    }
    bool isVideo(int delays){
      float dat = calc(_pin, delays);
      if(dat > 165){
        return true;
      }else{
        return false;
      }
    }
    int GetFrequency(){
      return _fs;
    }
    void loops(){
      if(_fs > 1500){
         _fs = 1000; 
      }
      if(!isVideo(50)){
         _fs += 10;
         SetFrequency(_fs);
         _isVideo = false;
      }else{
        _isVideo = true;
      }
    }
  private:
     bool SetFrequency(uint16_t f){
        _Delitel=f*8+3836; //считаем делитель
        _DelitelH=_Delitel>>8;
        _DelitelL=_Delitel&0XFF;
       Wire.beginTransmission(_addr);   // стучимся к синтезатору и передаем байты параметров
       Wire.write(_DelitelH);
       Wire.write(_DelitelL);
       Wire.write(0xCE);
       Wire.write(0x00);
       bool error = Wire.endTransmission();
       if(error == 0){
          return false;
       }else{
        return true;
       }
    }
    float calc(int pin,int delays){
      unsigned long times = millis();
      int d = 1;
      int newVal = 0;
      float dat = 0;
      while (d){
        newVal = analogRead(pin);
        if(newVal > 300 && newVal < 600){
          
          dat += 0.20;
        }
        if(millis() - times  >= delays){
          d = 0;
        }
      }
       return dat/(delays/50);
    }
     unsigned int _Delitel;
     byte _DelitelH;
     byte _DelitelL;
     int _pin;
     int _status = 0;
     byte _addr = 0x60;
};
