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

file(GLOB_RECURSE SOURCES "src/**/*.cpp" "src/**/*.hpp")

add_executable(
    ${PROJECT_NAME} 
    src/main.cpp
    ${SOURCES}
)
target_compile_options(${PROJECT_NAME} PRIVATE -O3)

add_executable(
  tests
  ${SOURCES}
  test/path_manipulations_test.cpp
)
target_link_libraries(tests GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(tests)