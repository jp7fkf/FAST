/**
  The MIT License (MIT)
  Copyright (c)  2020  Yudai Hashimoto

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

#ifndef __BEEP_H__
#define __BEEP_H__

#include <ESP8266WiFi.h>
#include <Ticker.h>

class Beep {
public:
  Beep(int pin_beep) : _pin_beep(pin_beep) {
    pinMode(_pin_beep, OUTPUT);
    unset();
    _is_on = false;
    _repeat = false;
    _repeat_state = false;
    _auto_off_second = 0;
  }

  void set();
  void unset();
  void setRepeat(int interval);
  void off();
  void setAutoOff(int auto_off_second);

  int getInterval() {
    return _interval;
  };
  bool getisOn() {
    return _is_on;
  };
  bool getRepeat() {
    return _repeat;
  };
  int getAutoOff() {
    return _auto_off_second;
  };

private:
  void repeat();

  Ticker tick_repeat;
  Ticker tick_auto_off;
  int _pin_beep;
  int _interval;
  int _auto_off_second;
  bool _is_on;
  bool _repeat;
  bool _repeat_state;
};

#endif
