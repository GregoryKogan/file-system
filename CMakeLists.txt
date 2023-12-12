cmake_minimum_required(VERSION 3.25)
project(file-system)

set(CMAKE_CXX_STANDARD 20)
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
    cli
    src/main.cpp
    ${SOURCES}
)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
  if(MSVC)
    target_compile_options(cli PRIVATE /O2)
  else()
    target_compile_options(cli PRIVATE -O3)
  endif()
elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
  if(MSVC)
    target_compile_options(cli PRIVATE /W4 /WX)
  else()
    target_compile_options(cli PRIVATE -Wall -Wextra -pedantic -Werror -g -fsanitize=address -fsanitize=undefined -fsanitize=unsigned-integer-overflow)
    target_link_libraries(cli -fsanitize=address -fsanitize=undefined -fsanitize=unsigned-integer-overflow)
  endif()
endif()

file(GLOB_RECURSE TESTS_SOURCES "test/*.cpp")
add_executable(
  tests
  ${SOURCES}
  ${TESTS_SOURCES}
)
target_link_libraries(tests GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(tests)