#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import requests
import time
import json

FAST_ENDPOINT = "http://172.16.0.31"

def main():
  itr = 0
  while True:
    red, green, blue = hue2rgb(240 - itr)
    _body = {
        "red": red,
        "green": green,
        "blue": blue,
        "flash": False
    }

    _res = requests.put(FAST_ENDPOINT + '/indicator', data=json.dumps(_body))
    print(str(_res.status_code) + ": " +_res.text)
    time.sleep(0.5)
    itr = itr + 5
    if itr > 240:
      itr = 0

def hue2rgb(hue):
  min = 0
  max = 100

  r = 0
  g = 0
  b = 0

  if 0 <= hue and hue < 60:
    r = max
    g = hue / 60 * (max - min) + min
    b = min
  elif 60 <= hue and hue < 120:
    r = (120 - hue) / 60 * (max - min) + min
    g = max
    b = min
  elif 120 <= hue and hue < 180:
    r = min
    g = max
    b = (hue - 120) / 60 * (max - min) + min
  elif 180 <= hue and hue < 240:
    r = min
    g = (240 - hue) / 60 * (max - min) + min
    b = max
  elif 240 <= hue and hue < 300:
    r = (hue - 240) / 60 * (max - min) + min
    g = min
    b = max
  elif 300 <= hue and hue < 360:
    r = max
    g = min
    b = (360 - hue) / 60 * (max - min) + min

  return r, g, b

if __name__=='__main__':
  main()
