/*
  FAST (Flexible Alerting Signal Tower)
  Opensource SignalTower with ESP8266 WiFi Module

  Author:  Yudai Hashimoto (https://jp7fkf.dev)
  Date:    2020.10.01

  Some sources are based on IR-Station(https://github.com/kerikun11/IR-Station)
  written by Mr.Ryotaro Okuni aka kerikun11 (Github: kerikun11). I'm deeply
  grateful to him.

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

#include "config.h"
#include "fast.h"
#include <ESP8266WiFi.h>

Fast fast(PIN_LED_R, PIN_LED_G, PIN_LED_B, PIN_BEEP);
volatile bool reset_flag = false;
unsigned int t = 0;

void IRAM_ATTR RESET() {
  reset_flag = true;
}

void IRAM_ATTR OFF() {
  if (millis() - t < 500 && millis() - t > 0)
    fast.indicatorOff();
  fast.beepOff();
  t = millis();
}

void setup() {
  Serial.begin(115200);
  delay(10);
  println_dbg("");
  println_dbg("Hello, I'm ESP-WROOM-02");

  fast.begin();
  pinMode(PIN_BUTTON, INPUT_PULLUP);
  attachInterrupt(PIN_BUTTON, RESET, RISING);
  pinMode(PIN_BUTTON_2, INPUT_PULLUP);
  attachInterrupt(PIN_BUTTON_2, OFF, RISING);

  println_dbg("Setup Completed");
}

void loop() {
  fast.handle();
  if (reset_flag)
    fast.reset();
}
