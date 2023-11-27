#include <fstream>
#include <iostream>
#include <utility>

#include "file_io/FileReader/FileReader.hpp"

auto main() -> int {
  auto ifs = std::make_unique<std::ifstream>("../src/assets/AnimeGirls.jpg",
                                             std::ios::binary);

  const std::uint32_t BLOCK_SIZE = 100;
  FileReader reader(std::move(ifs), FileHandler::FileOffset{0},
                    FileHandler::BlockSize{BLOCK_SIZE});

  while (true) {
    auto block = reader.read_next_block();

    for (auto byte : block) std::cout << static_cast<uint>(byte) << " ";
    std::cout << "\n";

    if (block.size() < reader.block_size()) break;
  }

  std::cout << "total read size: " << reader.handled_size() << "\n";

  return 0;
}