/**
  The MIT License (MIT)
  Copyright (c)  2020  Yudai Hashimoto
  Copyright (c)  2016  Ryotaro Onuki

  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "indicator.h"
#include "config.h"

void Indicator::setRgb(int val_red, int val_green, int val_blue) {
  setRed(val_red);
  setGreen(val_green);
  setBlue(val_blue);
}

void Indicator::setRed(int value) {
  if(value > 1023)
    value = 1023;
  if(value < 0)
    value = 0;
  _val_red = value;
  analogWrite(_pin_red, _val_red);
}

void Indicator::setGreen(int value) {
  if(value > 1023)
    value = 1023;
  if(value < 0)
    value = 0;
  _val_green = value;
  analogWrite(_pin_green, _val_green);
}

void Indicator::setBlue(int value) {
  if(value > 1023)
    value = 1023;
  if(value < 0)
    value = 0;
  _val_blue = value;
  analogWrite(_pin_blue, _val_blue);
}

void Indicator::off() {
  tick.detach();
  _flash = false;
  setRgb(0, 0, 0);
}

void Indicator::setFlash(int val_red, int val_green, int val_blue, int interval) {
  setRgb(val_red, val_green, val_blue);
  if (interval > 5000)
    interval = 5000;
  if (interval < 20)
    interval = 20;
  _interval = interval;
  _flash = true;
  tick.attach_ms(_interval, [this](){ this->flash(); });
}

void Indicator::flash(){
  if(_flash_state == true){
    analogWrite(_pin_red, 0);
    analogWrite(_pin_green, 0);
    analogWrite(_pin_blue, 0);
    _flash_state = false;
  }else if(_flash_state == false){
    setRgb(_val_red, _val_green, _val_blue);
    _flash_state = true;
  }
}
