#!/usr/bin/env python
# -*- coding: utf-8 -*-
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
from prettytable import PrettyTable
from colorama import init, Fore
init()


# 定义一个提取火车信息的类
class TicketCollection:
    header = '车次 车站 时间 历时 商务 一等 二等 软卧 硬卧 硬座 无座'.split()

    def __init__(self, trains, options):
        self.trains = trains
        self.options = options
        # 要将车站对应的字母编号改为文字，所以需要一个字典
        self.code_dict = {v: k for k, v in stations_dict.items()}

    def get_info(self):
        for train in self.trains:
            # print(train)
            data_list = train.split('|')
            train_num = data_list[3]
            # 这里可以排除掉没有在参数里面的列车类型
            if len(self.options) != 0 and (not ('-' + str(train_num[0].lower()) in self.options)):
                continue
            from_station_code = data_list[6]
            from_station_name = self.code_dict[from_station_code]
            to_station_code = data_list[7]
            to_station_name = self.code_dict[to_station_code]
            start_time = data_list[8]
            arrive_time = data_list[9]
            run_time = data_list[10]
            special_seat = data_list[32] or '--'
            first_seat = data_list[31] or '--'
            second_seat = data_list[30] or '--'
            soft_sleep = data_list[23] or '--'
            hard_sleep = data_list[28] or '--'
            hard_seat = data_list[29] or '--'
            no_seat = data_list[26] or '--'
            train = [train_num,
                     '\n'.join([Fore.GREEN + from_station_name + Fore.RESET, Fore.RED + to_station_name + Fore.RESET]),
                     '\n'.join([Fore.WHITE + start_time + Fore.RESET, Fore.YELLOW + arrive_time + Fore.RESET]),
                     run_time.replace(':', '小时') + '分',
                     special_seat,
                     first_seat,
                     second_seat,
                     soft_sleep,
                     hard_sleep,
                     hard_seat,
                     no_seat]
            # print(train)
            yield train

    def pretty_print(self):
        pt = PrettyTable(self.header)
        for train in self.get_info():
            pt.add_row(train)
        print(pt)


def cli():
    arguments = docopt(__doc__)
    # print(arguments)
    from_station = stations_dict.get(arguments['<from>'])
    to_station = stations_dict.get(arguments['<to>'])
    date = arguments['<date>']
    url = 'https://kyfw.12306.cn/otn/leftTicket/queryX?leftTicketDTO.train_date={}&leftTicketDTO.from_station={' \
          '}&leftTicketDTO.to_station={}&purpose_codes=ADULT'.format(
        date, from_station, to_station
    )
    # test_url = 'https://kyfw.12306.cn/otn/leftTicket/queryX?leftTicketDTO.train_date=2017-09-14&leftTicketDTO.from_station=BJP&leftTicketDTO.to_station=GZQ&purpose_codes=ADULT '
    # 添加一个请求头,猜测12306网站添加了识别浏览器的功能，直接使用url去请求会返回网络错误的html代码,调试了几天一直以为是编码的问题。
    # 解决方法是直接复制整个浏览器的请求头。
    headers = {
        'user-agent': 'Mozilla/5.0 (Windows NT 10.0) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/51.0.2704.79 '
                      'Safari/537.36 Edge/14.14393',
        'Accept': '*/*',
        'Accept-Encoding': 'gzip, deflate',
        'Accept-Language': 'en-US, en; q=0.8, zh-Hans-CN; q=0.5, zh-Hans; q=0.3',
        'Cache-Control': 'no-cache',
        'Connection': 'Keep-Alive',
        'Cookie': 'JSESSIONID=6864EE411B9930795020230322D61853; route=c5c62a339e7744272a54643b3be5bf64; '
                  'BIGipServerotn=2263351562.64545.0000; _jc_save_fromStation=%u5317%u4EAC%2CBJP; '
                  '_jc_save_toStation=%u5E7F%u5DDE%2CGZQ; _jc_save_fromDate=2017-09-15; _jc_save_toDate=2017-09-13; '
                  '_jc_save_wfdc_flag=dc; fp_ver=4.5.1; RAIL_EXPIRATION=1505578245698; '
                  'RAIL_DEVICEID=CdGe0zEvyzDYA3ig1XOzJI812FFL7zLfxu_ecLg_XPqr2SbyN6SBRQ0YYFsmE6FN7xMhh3qosDGhQsL'
                  '-eM2oworkS-cruz7_U0MPjbXrnllPzxIO_heq1Qil674n98GQx17903c2CJG0DETzu1xfmuN_7xWOoBEg',
        'Host': 'kyfw.12306.cn',
        'If-Modified-Since': '0',
        'Referer': 'https://kyfw.12306.cn/otn/leftTicket/init',
        'X-Requested-With': 'XMLHttpRequest'}
    r = requests.get(url, verify=False, headers=headers)
    # if r.status_code == requests.codes.ok:
    #    for k, v in r.json().items():
    #       print((k, v))
    # print(r.content)
    # print(r.status_code)
    # print(r.encoding)
    # r.encoding = 'utf-8'
    # print(r.encoding)
    # print(r.text)
    # 这里的接口经常会经常换，所以如果报错了就换一下接口。
    # print(r.content)
    # 打印响应头部
    # print(r.headers.get('Content-Encoding'))
    # print(r.json())
    left_tickets = r.json()['data']['result']
    options = ([key for key, value in arguments.items() if value is True])
    try:
        TicketCollection(left_tickets, options).pretty_print()
    except:
        try:
            print(r.json()['message'])
        except:
            print('Input Error')
    # print(left_tickets)


if __name__ == '__main__':
    cli()
