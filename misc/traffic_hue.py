#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import requests
import time
import json
import re
import math
from pysnmp.hlapi import *

FAST_ENDPOINT = "http://127.0.0.1"
SNMP_TARGET = '127.0.0.2'
SNMP_COMMUNITY = 'public'
INTERVAL = 5
INTENCITY_HIGH = 1023
INTENCITY_LOW = 512
BPS_HIGH = 200000000 #`200Mbps (100Mbps out + 100Mbps in)
BPS_LOW = 0 #0bps

def main():
    prev_octets_in = 0
    prev_octets_out = 0
    intencity_high = True

    while True:
      ifmetrics = getInterfaceMetricsSnmp()

      if prev_octets_out != 0:
        bps_in = (int(ifmetrics["4"]["ifInOctets"]) - prev_octets_in) * 8 / INTERVAL
        bps_out = (int(ifmetrics["4"]["ifOutOctets"]) - prev_octets_out) * 8 / INTERVAL
        print(f'in(bps): {human_readable(bps_in)}, out(bps): {human_readable(bps_out)}')
        hue = (bps_in + bps_out) / (BPS_HIGH - BPS_LOW)
        hue = 1 if hue > 1 else hue
        red, green, blue = hue2rgb(240 - hue*240)
        if intencity_high:
          red *= INTENCITY_HIGH
          green *= INTENCITY_HIGH
          blue *= INTENCITY_HIGH
          intencity_high = False
        else:
          red *= INTENCITY_LOW
          green *= INTENCITY_LOW
          blue *= INTENCITY_LOW
          intencity_high = True

        _body = {
            "red": red,
            "green": green,
            "blue": blue,
            "flash": False
        }
        _res = requests.put(FAST_ENDPOINT + '/indicator', data=json.dumps(_body))
        print(str(_res.status_code) + ": " +_res.text)

      prev_octets_in = int(ifmetrics["4"]["ifInOctets"])
      prev_octets_out = int(ifmetrics["4"]["ifOutOctets"])
      #print(json.dumps(ifmetrics["4"], indent=2))

      time.sleep(INTERVAL)

def human_readable(value):
  if math.log10(value) < 3:
    return f'{value}'
  elif math.log10(value) < 6:
    return f'{value/1000:.2f}k'
  elif math.log10(value) < 9:
    return f'{value/1000000:.2f}M'
  elif math.log10(value) < 12:
    return f'{value/1000000000:.2f}G'
  else:
    return f'{value/1000000000:.2f}G'

def getInterfaceMetricsSnmp():
  iterator = bulkCmd(SnmpEngine(),
                    CommunityData(SNMP_COMMUNITY),
                    UdpTransportTarget((SNMP_TARGET, 161)),
                    ContextData(),
                    0, 50,
                    ObjectType(ObjectIdentity('IF-MIB', 'interfaces')),
                    lexicographicMode=False)

  result = {}
  while True:
    try:
      errorIndication, errorStatus, errorIndex, varBinds = next(iterator)

      if errorIndication:
        print(errorIndication)
      elif errorStatus:
        print('%s at %s' % (errorStatus.prettyPrint(),
                            errorIndex and varBinds[int(errorIndex) - 1][0] or '?')
        )
      else:
        for varBind in varBinds:  # SNMP response contents
          _ifindex = re.search(r'[0-9]+$', varBind[0].prettyPrint()).group()
          _symbol = re.search(r'::(.*)\.[0-9]+$', varBind[0].prettyPrint()).group(1)
          result.setdefault(_ifindex, {})
          result[_ifindex][_symbol] = varBind[1].prettyPrint()
    except StopIteration:
      break
  return result

def hue2rgb(hue):
  min = 0
  max = 1

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
