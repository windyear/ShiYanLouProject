cmake_minimum_required(VERSION 3.8)
project(HttpServer C CXX)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp server_base.h server_http.h handler.h server_https.h)
add_executable(HttpServer ${SOURCE_FILES})
# 使用BOOST库
find_package(Boost)
IF (Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIR})
endif()
set (Boost_USE_STATIC_LIBS OFF) # enable dynamic linking
set (Boost_USE_MULTITHREAD ON)  # enable multithreading
find_package (Boost COMPONENTS REQUIRED  chrono filesystem system)
# 声明链接到BOOST库
target_link_libraries (HttpServer ${Boost_LIBRARIES})

find_package (Threads)
target_link_libraries (HttpServer ${CMAKE_THREAD_LIBS_INIT})

find_package(OpenSSL REQUIRED)
target_link_libraries(HttpServer OpenSSL::SSL)