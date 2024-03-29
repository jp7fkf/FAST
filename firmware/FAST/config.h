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

#ifndef __CONFIG_H__
#define __CONFIG_H__

/* Version */
#define FAST_VERSION "v0.9.2"

/* Hardware Mapping */
#define PIN_BUTTON (0)
#define PIN_BUTTON_2 (5)
#define PIN_BEEP (14)

#define PIN_LED_R (13)
#define PIN_LED_G (12)
#define PIN_LED_B (15)

// SSID & Password of ESP8266 Access Point Mode
#define SOFTAP_SSID "Fast"
#define SOFTAP_PASS "FastFast" // must be 8-32 chars

// WiFi connection Timeout
#define WIFI_CONNECT_TIMEOUT (10) // seconds

// WiFi auto recooect
#define WIFI_AUTO_RECONNECT true

// http://HOSTNAME_DEFAULT.local
#define HOSTNAME_DEFAULT "fast"

// OTA update
#define USE_OTA_UPDATE true

// Captive Portal
#define USE_CAPTIVE_PORTAL true

/* for Debug */
#define SERIAL_DEBUG true

#if SERIAL_DEBUG == true
#define DEBUG_SERIAL_STREAM Serial
#define print_dbg DEBUG_SERIAL_STREAM.print
#define printf_dbg DEBUG_SERIAL_STREAM.printf
#define println_dbg DEBUG_SERIAL_STREAM.println
#else
#define DEBUG_SERIAL_STREAM NULL
#define print_dbg   // No Operation
#define printf_dbg  // No Operation
#define println_dbg // No Operation
#endif

#endif
