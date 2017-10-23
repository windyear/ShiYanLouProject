//
// Created by windyear_office on 17-10-23.
//

#ifndef HTTPSERVER_SERVER_HTTP_H
#define HTTPSERVER_SERVER_HTTP_H

#include "server_base.h"

namespace WindyearWeb{
    typedef boost::asio::ip::tcp::socket HTTP;
    template<>
    class ServerHttp<HTTP>: public ServerBase<HTTP>{
    public:
        //构造函数,传入端口,线程数等参数
        ServerHttp(unsigned short port, size_t num_threads = 1):
                ServerBase(port, num_threads){};
    private:
        //实现 accept() 方法
        void accept(){
            //为连接创建一个新的 socket
            auto socket = std::make_shared<HTTP>(m_io_service);
            //使用 acceptor 函数接受连接
            acceptor.async_accept(*socket, [this, socket](const boost::system::error_code& ec){
               accept();
                if(!ec){
                    process_request_and_respond(socket);
                }
            });
        }
    };
}
#endif //HTTPSERVER_SERVER_HTTP_H
