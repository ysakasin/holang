cmake_minimum_required(VERSION 3.0)
project(holang)

set(CMAKE_CXX_STANDARD 11)

include_directories(include)
add_executable(ho lib/main.cpp lib/lexer.cpp lib/object.cpp lib/parser.cpp)
