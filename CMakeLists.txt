cmake_minimum_required(VERSION 3.0)
project(holang)

set(CMAKE_CXX_STANDARD 14)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(CMAKE_CXX_FLAGS_DEBUG -g)

set(PATH_HOLIB ${CMAKE_CURRENT_SOURCE_DIR}/holib)
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/include/config.hpp.in
                ${CMAKE_CURRENT_BINARY_DIR}/include/config.hpp)

include_directories(include)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/include)

add_subdirectory(lib)
add_subdirectory(src)
