cmake_minimum_required(VERSION 3.24)
project(fs)

set(CMAKE_CXX_STANDARD 17)

add_executable(
    fs 
    src/main.cpp

    src/FileSystem/FileSystem.hpp
    src/FileSystem/FileSystem.cpp
    src/FileSystem/DiskHandler/DiskHandler.hpp
    src/FileSystem/DiskHandler/DiskHandler.cpp
    src/FileSystem/DiskHandler/DiskReader/DiskReader.hpp
    src/FileSystem/DiskHandler/DiskReader/DiskReader.cpp
    src/FileSystem/DiskHandler/DiskWriter/DiskWriter.hpp
    src/FileSystem/DiskHandler/DiskWriter/DiskWriter.cpp
    src/FileSystem/Converter/Converter.hpp
    src/FileSystem/Converter/Converter.cpp
    src/FileSystem/FSMaker/FSMaker.hpp
    src/FileSystem/FSMaker/FSMaker.cpp
    src/FileSystem/FAT/FAT.hpp
    src/FileSystem/FAT/FAT.cpp
    src/FileSystem/FileData/FileData.hpp
    src/FileSystem/FileData/FileData.cpp
    src/FileSystem/Directory/Directory.hpp
    src/FileSystem/Directory/Directory.cpp
    src/FileSystem/FileHandler/FileHandler.hpp
    src/FileSystem/FileHandler/FileHandler.cpp
    src/FileSystem/FileHandler/FileReader/FileReader.hpp
    src/FileSystem/FileHandler/FileReader/FileReader.cpp
    src/FileSystem/FileHandler/FileWriter/FileWriter.hpp
    src/FileSystem/FileHandler/FileWriter/FileWriter.cpp
    src/FileSystem/PathResolver/PathResolver.hpp
    src/FileSystem/PathResolver/PathResolver.cpp
)

target_compile_options(
    fs PRIVATE
    -O3
)