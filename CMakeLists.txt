cmake_minimum_required(VERSION 3.10)
project(unique_ptr)

set(CMAKE_CXX_STANDARD 17)

if(DEFINED ENV{SHELL})
    set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address,undefined,leak")
endif()

add_executable(unique_ptr main.cpp unique_ptr.hpp)