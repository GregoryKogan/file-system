#include <fstream>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include "FileSystem/FileSystem.hpp"

void logic() {
  const std::string PATH = "test.fs";
  const uint64_t SIZE = 1024;
  const uint64_t CLUSTER_SIZE = 64;

  FileSystem::make(PATH, {SIZE, CLUSTER_SIZE});

  FileSystem file_system(PATH);
  std::cout << file_system;
}

auto main() -> int {
  try {
    logic();
  } catch (std::exception const &e) { std::cerr << e.what() << '\n'; }
}
