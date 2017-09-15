# -*- coding: utf-8 -*-

import scrapy


class TestScrapy(scrapy.spiders.Spider):
    name = "test"
    # allowed_domains = ['dmoztools.net']
    start_urls = ['https://bbs.hupu.com/20244361.html']

    def parse(self, response):
        filename = 'test'
        with open(filename, 'wb') as f:
            f.write(response.body)
