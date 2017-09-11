#!/usr/bin/env python
#-*- coding: utf-8 -*-

import re
import requests
from pprint import pprint

url = 'https://kyfw.12306.cn/otn/resources/js/framework/station_name.js?station_version=1.9025'
response = requests.get(url, verify = False)
stations = re.findall(u'([\u4e00-\u9fa5]+)\|([A-Z]+)', response.text)
stations_dict = dict(stations)
pprint(stations_dict, indent = 4)
#pprint(stations, indent = 4)
