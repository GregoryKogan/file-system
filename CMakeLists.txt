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
    src/file_io/FileWriter/FileWriter.hpp
    src/file_io/FileWriter/FileWriter.cpp
    src/FileSystem/FSMaker/FSMaker.hpp
    src/FileSystem/FSMaker/FSMaker.cpp
    src/FileSystem/FileSystem.hpp
    # src/FileSystem/FileSystem.cpp
)

target_compile_options(
    fs PRIVATE
    -O3
)