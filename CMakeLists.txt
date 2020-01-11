cmake_minimum_required(VERSION 3.16)
project(cpp_toolkit)

set(CMAKE_CXX_STANDARD 17)

include_directories(
        include
)


add_library(cpp-toolkit
        src/thread_pool.cpp
        )