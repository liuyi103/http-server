cmake_minimum_required(VERSION 3.6)
project(HTTPServer)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
include_directories(".")

set(SOURCE_FILES main.cpp MyHTTPRequest.cpp MyHTTPRequest.h)
add_executable(HTTPServer ${SOURCE_FILES})

set(TEST_FILES smalltest.cpp MyHTTPRequest.cpp MyHTTPRequest.h)
add_executable(smalltest ${TEST_FILES})

add_executable(benchmark benchmark.cpp)