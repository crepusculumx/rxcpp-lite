cmake_minimum_required(VERSION 3.21)
project(rxcpp-lite)

set(CMAKE_CXX_STANDARD 17)

file(GLOB RXCPP_LITE_SOURCES "sources/*.c++")
add_library(rxcpp-lite ${RXCPP_LITE_SOURCES})

target_include_directories(rxcpp-lite PUBLIC include/)