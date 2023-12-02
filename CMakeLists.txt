cmake_minimum_required(VERSION 3.24)
project(fs)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")

add_executable(
    ${PROJECT_NAME} 
    src/main.cpp

    src/CLI/CLI.hpp
    src/CLI/CLI.cpp
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
    ${PROJECT_NAME} PRIVATE
    -O3
)

add_executable(hello_test test/hello_test.cpp)
target_link_libraries(hello_test GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(hello_test)