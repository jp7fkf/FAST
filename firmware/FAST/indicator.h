/**
  The MIT License (MIT)
  Copyright (c)  2020  Yudai Hashimoto
  Copyright (c)  2016  Ryotaro Onuki

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions: The above copyright
  notice and this permission notice shall be included in all copies or
  substantial portions of the Software. THE SOFTWARE IS PROVIDED "AS IS",
  WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
  THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
  FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
  TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
  THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef __INDICATOR_H__
#define __INDICATOR_H__

#include <ESP8266WiFi.h>
#include <Ticker.h>

class Indicator {
public:
  Indicator(int pin_red, int pin_green, int pin_blue)
      : _pin_red(pin_red), _pin_green(pin_green), _pin_blue(pin_blue) {
    pinMode(_pin_red, OUTPUT);
    pinMode(_pin_green, OUTPUT);
    pinMode(_pin_blue, OUTPUT);
    setRgb(0, 0, 0);
    _flash = false;
    _flash_state = false;
    _auto_off_second = 0;
  }

  void setRgb(int val_red, int val_green, int val_blue);
  void setRed(int value);
  void setGreen(int value);
  void setBlue(int value);
  void off();
  void setFlash(int val_red, int val_green, int val_blue, int interval_ms);
  void setAuthOff(int auto_off_second);

  int getRed() {
    return _val_red;
  };
  int getGreen() {
    return _val_green;
  };
  int getBlue() {
    return _val_blue;
  };
  int getInterval() {
    return _interval_ms;
  };
  bool isOn() {
    return _val_red > 0 || _val_green > 0 || _val_blue > 0;
  };
  bool getFlash() {
    return _flash;
  };
  int getAutoOff() {
    return _auto_off_second;
  };

private:
  void flash();

  Ticker tick_flash;
  Ticker tick_auto_off;

  int _pin_red;
  int _pin_green;
  int _pin_blue;

  int _val_red;
  int _val_green;
  int _val_blue;
  int _interval_ms;
  int _auto_off_second;

  bool _flash;
  bool _flash_state;
};

#endif
