 /**
  The MIT License (MIT)
  Copyright (c)  2020  Yudai Hashimoto
  Copyright (c)  2016  Ryotaro Onuki

  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "fast.h"

#include <ArduinoJson.h>
#include <FS.h>
#include "file.h"
#include "wifi.h"
#include "ntp.h"

void Fast::begin(void) {
  yield();
  wdt_reset();
  indicator.setFlash(0, 0, 1023, 500);
  if (restore() == false) reset();

#if USE_OTA_UPDATE == true
  ota.begin(hostname);
#endif

  switch (mode) {
    case MODE_SETUP:
      println_dbg("Boot Mode: Setup");
      WiFi.mode(WIFI_AP_STA);
      setupAP(SOFTAP_SSID, SOFTAP_PASS);
      attachSetupApi();
      break;
    case MODE_STATION:
      println_dbg("Boot Mode: Station");
      WiFi.mode(WIFI_STA);
      if (is_static_ip) WiFi.config(local_ip, gateway, subnetmask);
      connectWifi(ssid, password, is_stealth_ssid);
      if (WiFi.localIP() != local_ip) {
        is_static_ip = false;
        save();
      }
      attachStationApi();
      ntp_begin();
      indicator.off();
      break;
    case MODE_AP:
      println_dbg("Boot Mode: Access Point");
      WiFi.mode(WIFI_AP_STA);
      setupAP(SOFTAP_SSID, SOFTAP_PASS);
      attachStationApi();
      save();
      indicator.off();
      break;
  }

  if (!MDNS.begin(hostname.c_str())) println_dbg("Error setting up MDNS responder!");
  else println_dbg("mDNS: http://" + hostname + ".local");

#if USE_CAPTIVE_PORTAL == true
  println_dbg("Starting Captive Portal...");
  dnsServer.start(DNS_PORT, "*", WiFi.softAPIP());
#endif

  println_dbg("Starting HTTP Updater...");
  httpUpdater.setup(&server, "/firmware");
  server.on("/description.xml", HTTP_GET, [this]() {
    displayRequest();
    SSDP.schema(server.client());
  });

  println_dbg("Starting HTTP Server...");
  server.begin();

  println_dbg("Starting SSDP...");
  SSDP.setSchemaURL("description.xml");
  SSDP.setHTTPPort(80);
  SSDP.setName(hostname);
  SSDP.setSerialNumber(String(ESP.getChipId() , HEX));
  SSDP.setURL("index.htm");
  SSDP.setModelName("FAST");
  SSDP.setModelNumber("v1.0.0");
  SSDP.setModelURL("https://github.com/jp7fkf/FAST");
  SSDP.setManufacturer("JP7FKF");
  SSDP.setManufacturerURL("https://jp7fkf.dev/");
  SSDP.begin();
}

void Fast::reset(bool clean) {
  version = FAST_VERSION;
  mode = MODE_SETUP;
  hostname = HOSTNAME_DEFAULT;

  is_stealth_ssid = false;
  ssid = "";
  password = "";

  www_username = "":
  www_password = "":

  is_static_ip = false;
  local_ip = 0U;
  subnetmask = 0U;
  gateway = 0U;

  save();
  ESP.reset();
}

void Fast::handle() {
  server.handleClient();
  ota.handle();
  switch (mode) {
    case MODE_SETUP:
      if ((WiFi.status() == WL_CONNECTED))
        indicator.setRgb(0, 1023, 1023);
#if USE_CAPTIVE_PORTAL == true
      dnsServer.processNextRequest();
#endif
      break;
    case MODE_STATION:
      static bool lost = false;
      if ((WiFi.status() != WL_CONNECTED)) {
        if (lost == false) {
          println_dbg("Lost WiFi: " + ssid);
          WiFi.mode(WIFI_AP_STA);
          delay(1000);
          setupAP(SOFTAP_SSID, SOFTAP_PASS);
          indicator.setRed(1023);
        }
        lost = true;
      } else {
        if (lost == true) {
          println_dbg("Found WiFi: " + ssid);
          WiFi.mode(WIFI_STA);
          indicator.setBlue(1023);
        }
        lost = false;
      }
      break;
    case MODE_AP:
#if USE_CAPTIVE_PORTAL == true
      dnsServer.processNextRequest();
#endif
      break;
  }
}

void Fast::beepOff() {
  beep.off();
}

void Fast::indicatorOff() {
  indicator.off();
}

bool Fast::restore() {
  yield();
  String s;
  if (getStringFromFile(SETTINGS_JSON_PATH, s) == false) return false;
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(s);
  if (!root.success())return false;

  version = (const char *)root["version"];
  mode = (int)root["mode"];
  hostname = (const char*)root["hostname"];

  is_stealth_ssid = (bool)root["is_stealth_ssid"];
  ssid = (const char*)root["ssid"];
  password = (const char*)root["password"];

  www_username = "admin"; // (const char*)root["www_username"];
  www_password = "admin"; // (const char*)root["www_password"];

  is_static_ip = (bool)root["is_static_ip"];
  local_ip = (const uint32_t)root["local_ip"];
  subnetmask = (const uint32_t)root["subnetmask"];
  gateway = (const uint32_t)root["gateway"];

  if (version != FAST_VERSION) {
    println_dbg("version difference");
    return false;
  }
  println_dbg("Restored Fast Settings");
  return true;
}

bool Fast::save() {
  yield();
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();

  root["version"] = version;
  root["mode"] = mode;
  root["hostname"] = hostname;

  root["is_stealth_ssid"] = is_stealth_ssid;
  root["ssid"] = ssid;
  root["password"] = password;

  root["is_static_ip"] = is_static_ip;
  root["local_ip"] = (const uint32_t)local_ip;
  root["subnetmask"] = (const uint32_t)subnetmask;
  root["gateway"] = (const uint32_t)gateway;

  String path = SETTINGS_JSON_PATH;
  SPIFFS.remove(path);
  File file = SPIFFS.open(path, "w");
  if (!file) {
    print_dbg("File open Error: ");
    println_dbg(path);
    return false;
  }
  root.printTo(file);
  print_dbg("File Size: ");
  println_dbg(file.size(), DEC);
  file.close();
  print_dbg("Backup successful: ");
  println_dbg(path);
  print_dbg("data: ");
  root.printTo(DEBUG_SERIAL_STREAM);
  return true;
}

void Fast::displayRequest() {
  yield();
  println_dbg("");
  println_dbg("New Request");
  print_dbg("URI: ");
  println_dbg(server.uri());
  print_dbg("Method: ");
  println_dbg(STR(server.method()));
  print_dbg("Arguments count: ");
  println_dbg(server.args(), DEC);
  for (uint8_t i = 0; i < server.args(); i++) {
    printf_dbg("\t%d = %d\n", server.argName(i).c_str(), server.arg(i).c_str());
  }
}

void Fast::attachSetupApi() {
  server.on("/wifi/list", [this]() {
    displayRequest();
    DynamicJsonBuffer jsonBuffer;
    JsonArray& root = jsonBuffer.createArray();
    int n = WiFi.scanNetworks();
    for (int i = 0; i < n; ++i) {
      String s = WiFi.SSID(i);
      if (s.length() < 28)root.add(s);
    }
    String res;
    root.printTo(res);
    println_dbg(res);
    server.send(200, "application/json", res);
    println_dbg("End");
  });
  server.on("/wifi/confirm", [this]() {
    displayRequest();
    if (WiFi.status() == WL_CONNECTED) {
      String res = (String)WiFi.localIP()[0] + "." + WiFi.localIP()[1] + "." + WiFi.localIP()[2] + "." + WiFi.localIP()[3];
      server.send(200, "text/palin", res);
      mode = MODE_STATION;
      local_ip = WiFi.localIP();
      subnetmask = WiFi.subnetMask();
      gateway = WiFi.gatewayIP();
      save();
      indicator.setBlue(1023);
      delay(1000);
      ESP.reset();
    } else {
      println_dbg("Not connected yet.");
      server.send(200, "text/plain", "false");
      println_dbg("End");
    }
  });
  server.on("/mode/station", [this]() {
    displayRequest();
    ssid = server.arg("ssid");
    password = server.arg("password");
    is_stealth_ssid = server.arg("stealth") == "true";
    hostname = server.arg("hostname");
    if (hostname == "") hostname = HOSTNAME_DEFAULT;
    print_dbg("Hostname: ");
    println_dbg(hostname);
    print_dbg("SSID: ");
    println_dbg(ssid);
    print_dbg("Password: ");
    println_dbg(password);
    print_dbg("Stealth: ");
    println_dbg(is_stealth_ssid ? "true" : "false");
    indicator.setGreen(1023);
    server.send(200);
    WiFi.disconnect();
    //    delay(1000);
    WiFi.begin(ssid.c_str(), password.c_str());
  });
  server.on("/mode/accesspoint", [this]() {
    displayRequest();
    server.send(200, "text / plain", "Setting up Access Point Successful");
    hostname = server.arg("hostname");
    if (hostname == "") hostname = HOSTNAME_DEFAULT;
    mode = MODE_AP;
    save();
    ESP.reset();
  });
  server.on("/dbg", [this]() {
    displayRequest();
    ssid = server.arg("ssid");
    password = server.arg("password");
    println_dbg("Target SSID : " + ssid);
    println_dbg("Target Password : " + password);
    indicator.setGreen(1023);
    if (connectWifi(ssid.c_str(), password.c_str())) {
      String res = (String)WiFi.localIP()[0] + "." + WiFi.localIP()[1] + "." + WiFi.localIP()[2] + "." + WiFi.localIP()[3];
      server.send(200, "text/plain", res);
    } else {
      server.send(200, "text/plain", "Connection Failed");
    }
  });
  server.onNotFound([this]() {
    println_dbg("Not Found");
    server.send(404, "text/plain", "Not Found");
    println_dbg("End");
//    displayRequest();
//    println_dbg("Redirect");
//    String res = "<script>location.href = \"http://" + (String)WiFi.softAPIP()[0] + "." + WiFi.softAPIP()[1] + "." + WiFi.softAPIP()[2] + "." + WiFi.softAPIP()[3] + "/\";</script>";
//    server.send(200, "text/html", res);
//    println_dbg("End");
  });
  server.serveStatic("/", SPIFFS, "/setup/");
}

void Fast::attachStationApi() {
  server.on("/info", [this]() {
    displayRequest();
    String res;
    if (getStringFromFile(SETTINGS_JSON_PATH, res)) {
      return server.send(200, "application/json", res);
    } else {
      return server.send(500, "text/plain", "Failed to open File");
    }
  });

  server.on("/wifi/disconnect", [this]() {
    displayRequest();
    server.send(200);
    delay(100);
    reset(false);
  });

  server.on("/wifi/change-ip", [this]() {
    displayRequest();
    IPAddress _local_ip, _subnetmask, _gateway;
    if (!_local_ip.fromString(server.arg("local_ip")) || !_subnetmask.fromString(server.arg("subnetmask")) || !_gateway.fromString(server.arg("gateway"))) {
      return server.send(400, "text/palin", "Bad Request!");
    }
    WiFi.config(_local_ip, _gateway, _subnetmask);
    if (WiFi.localIP() != _local_ip) {
      return server.send(500, "text/palin", "Couldn't change IP Address :(");
    }
    is_static_ip = true;
    local_ip = _local_ip;
    subnetmask = _subnetmask;
    gateway = _gateway;
    save();
    return server.send(200, "text/palin", "Changed IP Address to " + WiFi.localIP().toString());
  });

  server.on("/indicator", [this]() {
    displayRequest();
    if (!server.authenticate(www_username, www_password))
      return server.requestAuthentication(BASIC_AUTH);

    DynamicJsonBuffer jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(server.arg("plain"));
    switch(server.method()){
      case HTTP_PUT:
        indicatorOff();
        if (!root.success())
          return server.send(400, "text/plain", "Invalid Request");
        if (root["flash"] == true){
          indicator.setFlash( root["red"],
                              root["green"],
                              root["blue"],
                              root["interval"]);
        }else{
          indicator.setRgb( root["red"],
                            root["green"],
                            root["blue"]);
        }
        server.send(200, "application/json", "{\"red\":" + String(indicator.getRed()) + "," +
                                            "\"green\":" + String(indicator.getGreen()) + "," +
                                            "\"blue\":" + String(indicator.getBlue()) + "," +
                                            "\"interval\":" + String(indicator.getInterval()) + "," +
                                            "\"isOn\":" + String(BOOL_STR(indicator.isOn())) + "," +
                                            "\"flash\":" + String(BOOL_STR(indicator.getFlash())) + "}");
        println_dbg("End");
        break;
      case HTTP_GET:
        displayRequest();
      server.send(200, "application/json", "{\"red\":" + String(indicator.getRed()) + "," +
                                            "\"green\":" + String(indicator.getGreen()) + "," +
                                            "\"blue\":" + String(indicator.getBlue()) + "," +
                                            "\"interval\":" + String(indicator.getInterval()) + "," +
                                            "\"isOn\":" + String(BOOL_STR(indicator.isOn())) + "," +
                                            "\"flash\":" + String(BOOL_STR(indicator.getFlash())) + "}");
        println_dbg("End");
        break;
      default:
        return server.send(405, "text/plain", "Method Not Allowed");
    }
  });

  server.on("/beep", [this]() {
    displayRequest();
    if (!server.authenticate(www_username, www_password))
      return server.requestAuthentication(); // digest auth
    DynamicJsonBuffer jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(server.arg("plain"));
    switch(server.method()){
      case HTTP_PUT:
        beepOff();
        if (!root.success())
          return server.send(400, "text/plain", "Invalid Request");
        if (root["repeat"] == true){
          beep.setRepeat(root["interval"]);
        }else{
          if (root["isOn"] == true)
            beep.set();
          else
            beepOff();
        }
        server.send(200, "application/json", "{\"isOn\":" + String(BOOL_STR(beep.getisOn())) + "," +
                                            "\"repeat\":" + String(BOOL_STR(beep.getRepeat())) + "," +
                                            "\"interval\":" + String(beep.getInterval()) + "}");
        println_dbg("End");
        break;
      case HTTP_GET:
        displayRequest();
      server.send(200, "application/json", "{\"isOn\":" + String(BOOL_STR(beep.getisOn())) + "," +
                                            "\"repeat\":" + String(BOOL_STR(beep.getRepeat())) + "," +
                                            "\"interval\":" + String(beep.getInterval()) + "}");
        println_dbg("End");
        break;
      default:
        return server.send(405, "text/plain", "Method Not Allowed");
    }
  });

  server.onNotFound([this]() {
    displayRequest();
    println_dbg("Not Found");
    server.send(404, "text/plain", "Not Found");
    println_dbg("End");
//    String res = "<script>location.href = \"http://" + WiFi.localIP().toString() + "/\";</script>";
//    server.send(200, "text/html", res);
//    println_dbg("End");
  });

  server.serveStatic("/", SPIFFS, "/main/", "public");
}
