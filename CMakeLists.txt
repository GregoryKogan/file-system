cmake_minimum_required(VERSION 3.24)
project(fs)

set(CMAKE_CXX_STANDARD 17)

add_executable(
    fs 
    src/main.cpp

    src/FileSystem/file_io/FileHandler.hpp
    src/FileSystem/file_io/FileHandler.cpp
    src/FileSystem/file_io/FileReader/FileReader.hpp
    src/FileSystem/file_io/FileReader/FileReader.cpp
    src/FileSystem/file_io/FileWriter/FileWriter.hpp
    src/FileSystem/file_io/FileWriter/FileWriter.cpp
    src/FileSystem/Converter/Converter.hpp
    src/FileSystem/Converter/Converter.cpp
    src/FileSystem/FSMaker/FSMaker.hpp
    src/FileSystem/FSMaker/FSMaker.cpp
    src/FileSystem/FileSystem.hpp
    src/FileSystem/FileSystem.cpp
)

target_compile_options(
    fs PRIVATE
    -O3
)