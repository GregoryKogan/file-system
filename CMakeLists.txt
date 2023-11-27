cmake_minimum_required(VERSION 3.24)
project(fs)

set(CMAKE_CXX_STANDARD 17)

add_executable(
    fs 
    src/main.cpp

    src/file_io/FileHandler.hpp
    src/file_io/FileHandler.cpp
    src/file_io/FileReader/FileReader.hpp
    src/file_io/FileReader/FileReader.cpp
)
