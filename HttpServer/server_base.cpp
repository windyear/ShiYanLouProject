//
// Created by windyear_office on 17-10-20.
//
#include "server_base.h"

void WindyearWeb::ServerBase::start() {
    //将请求资源放到 vector 中,默认资源放到最后,因为找不到匹配的请求路径是才使用默认资源
    for(auto it = resource.begin(); it != resource.end(); it++){
        all_resources.push_back(it);
    }
    for(auto it = default_resource.begin(); it != default_resource.end(); it++){
        all_resources.push_back(it);
    }

    //调用 accept() 处理连接
    accept();

    //运行 (num_threads-1) 作为线程池
    for(size_t c = 1; c < num_threads; c++){
        threads.emplace_back([this](){
            m_io_service.run();
        });
    }

    //主线程
    m_io_service.run();

    //等待其他线程
    for(auto & t: threads)
        t.join();
}
