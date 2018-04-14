cmake_minimum_required(VERSION 3.0)
project(holang)

set(CMAKE_CXX_STANDARD 14)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(CMAKE_CXX_FLAGS_DEBUG -g)

include_directories(include)

add_subdirectory(lib)
add_subdirectory(src)
