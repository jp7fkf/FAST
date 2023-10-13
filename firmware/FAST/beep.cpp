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

#include "beep.h"
#include "config.h"

void Beep::set() {
  _is_on = true;
  digitalWrite(_pin_beep, HIGH);
}

void Beep::unset() {
  _is_on = false;
  digitalWrite(_pin_beep, LOW);
}

void Beep::setRepeat(int interval) {
  if (interval > 5000)
    interval = 5000;
  if (interval < 20)
    interval = 20;
  _interval = interval;
  _is_on = true;
  _repeat = true;
  tick.attach_ms(_interval, [this]() {
    this->repeat();
  });
}

void Beep::off() {
  tick.detach();
  _repeat = false;
  unset();
}

void Beep::repeat() {
  if (_repeat_state == true) {
    digitalWrite(_pin_beep, LOW);
    _repeat_state = false;
  } else if (_repeat_state == false) {
    set();
    _repeat_state = true;
  }
}
