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

  FileSystem::Settings settings{.size = SIZE, .cluster_size = CLUSTER_SIZE};
  FileSystem::make_fs(PATH, settings);
}

auto main() -> int {
  try {
    logic();
  } catch (std::exception const &e) { std::cerr << e.what() << '\n'; }
}
