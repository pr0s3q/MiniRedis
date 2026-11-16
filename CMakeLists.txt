cmake_minimum_required(VERSION 4.4)
project(MiniRedis)

set(CMAKE_CXX_STANDARD 26)

option(LOGS "Enable logs" ON)

add_executable(MiniRedis main.cpp)

if(LOGS)
    target_compile_definitions(MiniRedis PRIVATE LOGS)
endif()
