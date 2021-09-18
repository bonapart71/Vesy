//#pragma once
//#include <Arduino.h>

class BonLed {

  private:
    uint8_t _pin;
    bool _ledIsOn;
    bool _blinkModeOn;
    unsigned long _timeout;
    unsigned long _lastTime;

  public:
    BonLed(uint8_t pin) {
      _pin = pin;
      pinMode(pin, OUTPUT);
      _ledIsOn = false;
      digitalWrite(_pin, LOW);
    };


    void on() {
      digitalWrite(_pin, HIGH);
      _ledIsOn = true;
      _blinkModeOn = false;
    };

    void off() {
      digitalWrite(_pin, LOW);
      _ledIsOn = false;
      _blinkModeOn = false;
    };

    void blink(unsigned long timeout = 500) {
      _timeout = timeout;
      _blinkModeOn = true;
      digitalWrite(_pin, HIGH);
      _ledIsOn = true;
    };

    unsigned long timeout(){
      return _timeout;
    }

    bool isOn() {
      return _ledIsOn;
    };

    bool isBlinking() {
      return _blinkModeOn;
    };

    //Led Status - 0-off, 1-on, 2-blinking
    byte ledStatus(){
      if (_blinkModeOn) return 2;
      if (_ledIsOn) return 1;
      return 0;
    }

    void work(unsigned long time) {
      if (_blinkModeOn) {
        if (time - _lastTime > _timeout) {
          if (_ledIsOn) {
            digitalWrite(_pin, HIGH);
          }
          else {
            digitalWrite(_pin, LOW);
          }
          _ledIsOn = !_ledIsOn;
          _lastTime = time;
        }
      }


    };




};
