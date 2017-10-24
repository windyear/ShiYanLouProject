//
// Created by windyear_office on 17-10-24.
//

#ifndef HTTPSERVER_SERVER_HTTPS_H
#define HTTPSERVER_SERVER_HTTPS_H

#include "server_base.h"
#include <boost/asio/ssl.hpp>


namespace WindyearWeb{
    //定义 https 类型
    typedef boost::asio::ssl::stream<boost::asio::ip::tcp::socket> HTTPS;

    //定义 https 类,模板类型为 HTTPS
    template<>
    class ServerHttp<HTTPS>: public ServerBase<HTTPS>{
    public:
        //构造函数,拥有证书文件和私匙文件两个参数
        ServerHttp(unsigned short port, size_t num_threads,
        const std::string& cert_file, const std::string& private_key_file):
                ServerBase<HTTPS>::ServerBase(port, num_threads),context(boost::asio::ssl::context::sslv23){
            //使用证书文件
            context.use_certificate_chain_file(cert_file);
            //使用私匙文件
            context.use_private_key_file(private_key_file, boost::asio::ssl::context::pem);
        }
    private:
        //定义一个 ssl context 对象
        boost::asio::ssl::context context;
        //实现 accept 方法
        void accept(){
            //指向 HTTPS 的指针
            auto socket = std::make_shared<HTTPS>(m_io_service, context);
            acceptor.async_accept((*socket).lowest_layer(),
            [this, socket](const boost::system::error_code& ec){
                //这里递归调用 accept ,接受连接
                accept();
                if(!ec){
                    (*socket).async_handshake(boost::asio::ssl::stream_base::server,
                    [this, socket](const boost::system::error_code& ec){
                       if(!ec){
                           process_request_and_respond(socket);
                       }
                    });
                }
            });
        }

    };
}
#endif //HTTPSERVER_SERVER_HTTPS_H
