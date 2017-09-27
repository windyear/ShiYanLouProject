# -*- coding: utf-8 -*-

# Define your item pipelines here
#
# Don't forget to add your pipeline to the ITEM_PIPELINES setting
# See: http://doc.scrapy.org/en/latest/topics/item-pipeline.html
# import os
# import re

class TestscrapyPipeline(object):
    def process_item(self, item, spider):
        # 保存文件路径
        # database = "./data"
        # if not os.path.exists(database):
        #     os.makedirs(database)
        # pattern_html = re.compile(r'''/(\d+).html''')
        # file_name_number = ''
        # for number in re.findall(pattern_html, item['title']):
        #     file_name_number = file_name_number + number
        # 判断图片格式，然后根据图片格式保存图片
        with open("图片链接.txt", 'a') as f:
                f.write(str(item['link']))
                f.write('\n')
        return item
