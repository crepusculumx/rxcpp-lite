cmake_minimum_required(VERSION 3.21)
project(rxcpp-lite)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)

file(GLOB RXCPP_LITE_SOURCES "include/rxcpp-lite/*.hpp")

add_library(rxcpp-lite INTERFACE)
target_sources(rxcpp-lite INTERFACE ${RXCPP_LITE_SOURCES})
target_include_directories(rxcpp-lite INTERFACE include/
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        $<INSTALL_INTERFACE:include/rxcpp-lite>)
