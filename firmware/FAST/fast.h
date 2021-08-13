/**
  The MIT License (MIT)
  Copyright (c)  2020  Yudai Hashimoto
  Copyright (c)  2016  Ryotaro Onuki

  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef __FAST_H__
#define __FAST_H__

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include <ESP8266SSDP.h>
#include <ESP8266mDNS.h>
#include <DNSServer.h>
#undef max(a,b)
#include <vector>
#include "config.h"
#include "indicator.h"
#include "beep.h"
#include "ota.h"

#define MODE_SETUP   0
#define MODE_STATION 1
#define MODE_AP      2

#define STR(var)
#define BOOL_STR(b) ((b) ? "true" :"false")

#define SETTINGS_JSON_PATH       ("/settings.json")

const int DNS_PORT = 53;
const int HTTP_PORT = 80;

class Fast {
  public:
    Fast(int pin_beep, int pin_red, int pin_green, int pin_blue):
      beep(pin_beep), indicator(pin_red, pin_green, pin_blue), server(HTTP_PORT), httpUpdater(true) {
    }
    void begin();
    void reset(bool clean = true);
    void handle();
    void beepOff();
    void indicatorOff();

  private:
    String version;
    uint8_t mode;
    String hostname;

    bool is_stealth_ssid;
    String ssid;
    String password;

    String www_username;
    String www_password;

    bool is_static_ip;
    IPAddress local_ip;
    IPAddress gateway;
    IPAddress subnetmask;

    Indicator indicator;
    Beep beep;
    ESP8266WebServer server;
    ESP8266HTTPUpdateServer httpUpdater;
    DNSServer dnsServer;
    OTA ota;

    bool restore();
    bool save();

    void displayRequest();
    void attachSetupApi();
    void attachStationApi();
};

#endif
