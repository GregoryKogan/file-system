cmake_minimum_required(VERSION 3.24)
project(fs)

set(CMAKE_CXX_STANDARD 17)

add_executable(
    fs 
    src/main.cpp

    src/Reader/Reader.hpp
    src/Reader/Reader.cpp
)
