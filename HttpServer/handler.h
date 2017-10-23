//
// Created by windyear_office on 17-10-23.
//

#ifndef HTTPSERVER_HANDLER_H
#define HTTPSERVER_HANDLER_H

#include "server_base.h"
#include <fstream>

using namespace std;
using namespace WindyearWeb;

//一个模板函数,用于启动服务器
template<typename SERVER_TYPE>
void start_server(SERVER_TYPE& server){
    //增加请求资源的处理办法
    //先读取内容到 streambuf
    //处理 /string 的 POST 请求,返回 POST　的方法
    server.resource["^/string/?$"]["POST"] = [](ostream& response, Request& request){
        stringstream ss;
        *request.content >> ss.rdbuf();
        string content = ss.str();

        //直接返回请求结果
        response << "HTTP/1.1 200 OK\r\nContent-Length: " << content.length() << "\r\n\r\n" << content;
    };
    //处理 /info 的 GET 请求,返回请求的格式
    server.resource["^/info/?$"]["GET"] = [](ostream& response, Request& request){
        stringstream content_stream;
        content_stream << "<h1>Request:</h1>";
        content_stream << request.method << "" << request.path << " HTTP/" << request.http_version << "<br>";
        for(auto & header: request.header){
            content_stream << header.first << " " << header.second << "<br>";
        }
        //这里使用了流指针的位置来获取内容的长度
        content_stream.seekp(0, ios::end);
        response << "HTTP/1.1 200 OK\r\nContent-Length: " << content_stream.tellp() << "\r\n\r\n" << content_stream.rdbuf();
    };

    //处理 match 的 GET 请求
    server.resource["^/match/([0-9a-zA-Z]+)/?$"]["GET"] = [](ostream& response, Request& request){
        string number = request.path_match[1];
        response << "HTTP/1.1 200 OK\r\nContent-Length: " << number.length() << "\r\n\r\n" << number;
    };
    //默认处理请求,应答 web 目录下面的文件,默认文件是 index.html
    server.default_resource["^/?(.*)$"]["GET"] = [](ostream& response, Request& request){
        string filename = "web/";

        string path = request.path_match[1];
        // 防止使用 '..' 来访问 web/ 目录外的内容,删除 '.'
        //最后的一个 '.' 的位置
        size_t last_pos = path.rfind(".");
        size_t current_pos = 0;
        size_t pos;
        while((pos = path.find('.', current_pos)) != string::npos && pos != last_pos){
            current_pos = pos;
            path.erase(pos, 1);
            last_pos--;
        }

        filename += path;
        //文件流,用来读取文件到文件输入流
        ifstream ifs;
        //文件或目录检查
        if(filename.find('.') == string::npos){
            if(filename[filename.length() - 1] != '/'){
                filename += '/';
            }
            filename += "index.html";
        }

        ifs.open(filename, ifstream::in);
        //获取长度
        if(ifs){
            ifs.seekg(0, ios::end);
            size_t length = ifs.tellg();

            ifs.seekg(0, ios::beg);

            //拷贝文件内容
            response << "HTTP/1.1 200 OK\r\nContent-Length: " << length << "\r\n\r\n" << ifs.rdbuf();
            ifs.close();
        }else{
            // 文件打开失败,返回信息
            string content = "Could not open file " + filename;
            response << "HTTP/1.1 200 OK\r\nContent-Length: " << content.length() << "\r\n\r\n" << content;
        }
    };
    server.start();

}
#endif //HTTPSERVER_HANDLER_H
