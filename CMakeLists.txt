cmake_minimum_required(VERSION 3.25)
project(fs)

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
    ${PROJECT_NAME} 
    src/main.cpp
    ${SOURCES}
)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
  if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /O2)
  else()
    target_compile_options(${PROJECT_NAME} PRIVATE -O3)
  endif()
endif()

add_executable(
  tests
  ${SOURCES}
  test/dirname_basename.cpp
  test/converter.cpp
  test/directory.cpp
  test/fat.cpp
)
target_link_libraries(tests GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(tests)