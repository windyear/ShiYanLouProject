//
// Created by windyear_office on 17-10-20.
//

#ifndef HTTPSERVER_SERVER_BASE_H
#define HTTPSERVER_SERVER_BASE_H

#include <unordered_map>
#include <thread>
#include <regex>
#include <vector>
#include <boost/asio.hpp>

namespace WindyearWeb{
    //定义一个结构处理请求
    struct Request{
        //请求的信息如路径,方法等
        std::string path, method, http_version;
        //只能指正指向请求内容
        std::shared_ptr<std::istream> content;
        //哈希容器,存储 key-value 对
        std::unordered_map<std::string, std::string> header;
        //正则表达式处理路径匹配
        std::smatch path_match;
    };

    //定义一个复杂的资源类型,多个 map 嵌套
    typedef std::map<std::string, std::unordered_map<std::string,
            std::function<void(std::ostream &, Request &)>>> resource_type;

    //定义一个类模板,传入 socket 的类型作为模板参数
    template <typename socket_type>
    class ServerBase{
    public:
        //服务器访问资源处理方式
        resource_type resource;
        //默认处理方式
        resource_type default_resource;
        //构造服务器,初始化端口,默认使用一个线程
        ServerBase(unsigned short port, size_t num_threads = 1):
        endpoint(boost::asio::ip::tcp::v4(), port),
        acceptor(m_io_service, endpoint),
        num_threads(num_threads){}

        //start the server
        void start();
    protected:
        //用于内部处理不同的资源请求类型
        std::vector<resource_type::iterator> all_resources;
        //虚函数实现的服务端 accept, 为了实现不同类型的服务器
        //用于处理 TCP socket 连接的 BoostAsio 库对象
        boost::asio::io_service m_io_service;
        boost::asio::ip::tcp::endpoint endpoint;
        boost::asio::ip::tcp::acceptor acceptor;

        //服务器线程
        size_t num_threads;
        //vector存储多个线程
        std::vector<std::thread> threads;

        virtual void accept();
        //具体的处理请求和响应的函数, 参数是不同的 socket 类型的 socket 指针
        //函数后面的 const 表明该成员函数不能修改 no static 变量
        void process_request_and_respond(std::shared_ptr<socket_type> socket) const;
    };

    //一个继承服务器基类的服务器类
    template <typename socket_type>
    class Server: public ServerBase<socket_type>{};

}
#endif //HTTPSERVER_SERVER_BASE_H
