cmake_minimum_required(VERSION 4.4)
project(MiniRedis)

set(CMAKE_CXX_STANDARD 26)

option(LOGS "Enable logs" ON)

include_directories(MiniRedis PRIVATE ${CMAKE_SOURCE_DIR}/include)

file(GLOB SOURCES "${CMAKE_SOURCE_DIR}/src/*.cpp")

add_executable(MiniRedis main.cpp ${SOURCES})

if(LOGS)
    target_compile_definitions(MiniRedis PRIVATE LOGS)
endif()
