import requests
def cli():
    test_url = 'https://kyfw.12306.cn/otn/leftTicket/queryX?leftTicketDTO.train_date=2017-09-14&leftTicketDTO.from_station=BJP&leftTicketDTO.to_station=GZQ&purpose_codes=ADULT'
    r = requests.get(test_url, verify=False)
    print(r.text)
