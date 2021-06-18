cmake_minimum_required(VERSION 3.0)
project(test)

find_package(libwebsockets CONFIG REQUIRED)

add_executable(server server.cpp)

target_link_libraries(server PRIVATE websockets)