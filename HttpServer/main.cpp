#include <iostream>
#include "server_http.h"
#include "server_https.h"
#include "handler.h"

using namespace WindyearWeb;

int main() {
//    std::cout << "The http server is started:\n ......" << std::endl;
//    // 直接构造服务器对象,传入的是端口还有线程数量
//    ServerHttp<HTTP> server(12345, 4);
//    start_server<ServerHttp<HTTP>>(server);

    //下面构造的是 https 服务器的对象和启动服务器
    std::cout << "The https server is started:\n ......" << std::endl;
    ServerHttp<HTTPS> server_http(12345, 4, "server.crt", "server.key");
    start_server<ServerHttp<HTTPS>>(server_http);
    return 0;
}