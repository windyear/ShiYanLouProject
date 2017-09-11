#!/usr/bin/env python
#-*- codeing: utf-8 -*-
"""命令行解释库试用程序，火车票查询工具

Usage:
    tickets [-gdtkz] <from> <to> <date>
    
Options:
    -h,--help  显示帮助菜单
    -g         高铁
    -d         动车
    -t         特快
    -k         快速
    -z         直达
    
Example:
    tickets 北京 上海 2017-9-1
    tickets -tk 广州 肇庆 2017-9-2
"""

from docopt import docopt
from stations_dict import stations_dict
import requests

def cli():
    arguments = docopt(__doc__)
    #print(arguments)
    from_station = stations_dict.get(arguments['<from>'])
    to_station = stations_dict.get(arguments['<to>'])
    date = arguments['<date>']
    url = 'https://kyfw.12306.cn/otn/leftTicket/query?leftTicketDTO.train_date={}&leftTicketDTO.from_station={}&leftTicketDTO.to_station={}&purpose_codes=ADULT'.format(
        date, from_station, to_station
    )
    r = requests.get(url, verify = False)
    print(r.json())

if __name__ == '__main__':
    cli()

    