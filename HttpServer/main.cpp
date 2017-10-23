#include <iostream>
#include "server_http.h"
#include "handler.h"

using namespace WindyearWeb;

int main() {
    std::cout << "Hello, World!" << std::endl;
    // 直接构造服务器对象,传入的是端口还有线程数量
    ServerHttp<HTTP> server(12345, 4);
    start_server<ServerHttp<HTTP>>(server);
    return 0;
}