cmake_minimum_required(VERSION 3.0)
project(holang)

set(CMAKE_CXX_STANDARD 14)

include_directories(include)
set(holang_src
    lib/main.cpp
    lib/lexer.cpp
    lib/object.cpp
    lib/parser.cpp
)

add_executable(ho ${holang_src})
