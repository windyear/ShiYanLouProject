# -*- coding: utf-8 -*-

import scrapy
# import re
from testscrapy.items import TestscrapyItem
from scrapy.http import Request


class TestScrapy(scrapy.spiders.Spider):
    name = "testscrapy"
    allowed_domains = ['hupu.com']
    start_urls = ['https://bbs.hupu.com/bxj-' + str(i) for i in range(10)]

    def __init__(self):
        self.base_url = "https://bbs.hupu.com"
        self.page_number = 1

    # def parse(self, response):
    #
    #     filename = str(self.page_number) + 'test'
    #     # 每一次爬取一页之后页数加一
    #     self.page_number = self.page_number + 1
    #     # 获取步行街论坛每一页里面的所有帖子链接，然后再保存到文件中
    #     item = TestscrapyItem()
    #     # 获取到一个由链接和帖子组成的列表
    #     links_list = response.xpath('//div[@class = "titlelink box"]/a[contains(@href, ".html")]').extract()
    #     # 用正则表达式提取出链接
    #     # 直接使用xpath比正则表达式简单
    #     find_link_list = []
    #     # 选取带有.html的href属性的元素
    #     find_title_list = response.xpath('//div[@class = "titlelink box"]/a[contains(@href, ".html")]/text()').extract()
    #     pattern_html = re.compile(r'''href="(/\d+.html)''')
    #     for link in links_list:
    #         for find_link in re.findall(pattern_html, link):
    #             find_link_list.append(self.base_url + find_link)
    #     links_to_write = ''
    #     titles_to_write = ''
    #     for link in find_link_list:
    #         links_to_write = links_to_write + link + '\n'
    #     for title in find_title_list:
    #         titles_to_write = titles_to_write + title + '\n'
    #     with open(filename, 'wb') as f:
    #         f.write(bytes(links_to_write + titles_to_write, encoding = 'utf-8'))

    def parse(self, response):
        '''分情况处理请求返回的结果
        如果是步行街的帖子就获取所有帖子链接进行爬取，
        如果是帖子的就爬取标题中带有‘福利’字眼的图片链接
        并存到 item 中，然后通过pipelines进行下载'''
        is_bxj_url = (len(response.url.split('.')) == 3)
        if is_bxj_url:
            # 所有的链接
            links_list = response.xpath('//div[@class = "titlelink box"]/a[contains(@href, ".html")]/@href').extract()
            for link in links_list:
                yield Request(self.base_url + link, callback = self.parse)
        else:
            img_list = response.xpath('//div[@class = "quote-content"]/p/img/@src').extract()
            item = TestscrapyItem()
            item['title'] = response.url
            item['link'] = img_list
            item['image_urls'] = img_list
            if len(img_list) != 0:
                yield item