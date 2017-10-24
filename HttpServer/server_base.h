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
        //智能指针指向请求内容
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
        void start() {
            //将请求资源放到 vector 中,默认资源放到最后,因为找不到匹配的请求路径是才使用默认资源
            for(auto it = ServerBase::resource.begin(); it != resource.end(); it++){
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
        //刚开始的时候声明虚函数忘记加大括号,导致该声明无效,子类实现的是自己的 accept 函数
        virtual void accept(){};
        //具体的处理请求和响应的函数, 参数是不同的 socket 类型的 socket 指针
        //函数后面的 const 表明该成员函数不能修改 no static 变量
        void process_request_and_respond(std::shared_ptr<socket_type> socket) const {
        //用来缓存读取出来的内容
            auto read_buffer = std::make_shared<boost::asio::streambuf>();
            //使用 async_read_until 函数来读取到终止符的内容,然后在匿名函数里面进行处理
            boost::asio::async_read_until(*socket, *read_buffer,"\r\n\r\n",
            [this, socket, read_buffer](const boost::system::error_code& ec, size_t bytes_transferred){
                if(!ec){
                    size_t total = read_buffer->size();
                    std::istream stream(read_buffer.get());
                    auto request = std::make_shared<Request>();
                    //下面要完成将 stream 中的信息进行解释并保存到 request 对象中的操作
                    *request = parse_request(stream);

                    size_t num_additional_bytes = total - bytes_transferred;
                    //如果请求含有内容,继续读取内容
                    if(request->header.count("Content-Length") > 0){
                        //这次使用 async_read 函数,读取到一定字节
                        boost::asio::async_read(*socket, *read_buffer,
                        boost::asio::transfer_exactly(std::stoll(request->header["Content-Length"]) - num_additional_bytes),
                        [this, socket, read_buffer, request](const boost::system::error_code& ec, size_t bytes_transferred){
                            if(!ec){
                                request->content = std::shared_ptr<std::istream>(new std::istream(read_buffer.get()));
                                respond(socket, request);
                            }
                        });
                    }else{
                        respond(socket, request);
                    }
                }
            });
            }

        //一个用于处理请求信息的函数
        Request parse_request(std::istream& stream) const {
            Request request;
            //用正则表达式匹配出 http 请求的第一行,请求分为请求行,请求头,空行,消息体组成
            //正则表达式的小括号可以算作一个匹配项
            // ^ 在中括号里面可以表示除了后面跟着的内容
            std::regex e("^([^ ]*) ([^ ]*) HTTP/([^ ]*)$");
            std::string line;
            getline(stream, line);
            //出去最后一个回车
            line.pop_back();
            std::smatch sub_match;
            if(std::regex_match(line, sub_match, e)){
                request.method = sub_match[1];
                request.path = sub_match[2];
                request.http_version = sub_match[3];

                //解析头部其他信息
                bool matched;
                e = "^([^:]*): ?(.*)$";
                do{
                    getline(stream, line);
                    line.pop_back();
                    matched = std::regex_match(line, sub_match, e);
                    if(matched){
                        request.header[sub_match[1]] = sub_match[2];
                    }
                }while(matched == true);
            }
            return request;
        }

        //用于服务器 respond 的函数
        void respond(std::shared_ptr<socket_type> socket, std::shared_ptr<Request> request) const{
            //对请求与方法进行匹配,生成响应
            for(auto res_it: all_resources){
                std::regex e(res_it->first);
                std::smatch sm_res;
                if(std::regex_match(request->path, sm_res, e)){
                    if(res_it->second.count(request->method) > 0) {
                        request->path_match = move(sm_res);
                        auto write_buffer = std::make_shared<boost::asio::streambuf>();
                        //存储响应内容的输出流对象
                        std::ostream response(write_buffer.get());
                        //调用 resource_type 的一个函数, 传入请求和响应
                        res_it->second[request->method](response, *request);

                        //写响应函数
                        boost::asio::async_write(*socket, *write_buffer,
                                                 [this, socket, request, write_buffer](
                                                         const boost::system::error_code &ec,
                                                         size_t bytes_transferred) {
                                                     if (!ec && stof(request->http_version) > 1.05)
                                                         process_request_and_respond(socket);

                                                 });
                        return;
                    }
                }
            }
        }
        };
    //一个继承服务器基类的服务器类
    template <typename socket_type>
    class ServerHttp: public ServerBase<socket_type>{};

}
#endif //HTTPSERVER_SERVER_BASE_H
