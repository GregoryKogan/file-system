#include <fstream>
#include <iostream>
#include <utility>

#include "Reader/Reader.hpp"

auto main() -> int {
  auto ifs = std::make_unique<std::ifstream>("../src/assets/AnimeGirls.jpg",
                                             std::ios::binary);

  const std::uint32_t BLOCK_SIZE = 100;
  Reader reader(std::move(ifs), FileOffset{0}, ReadBlockSize{BLOCK_SIZE});

  while (true) {
    auto block = reader.read_next_block();
    for (auto byte : block) {
      std::cout << static_cast<uint>(byte) << " ";
    }
    std::cout << "\n";

    if (block.size() < reader.block_size().value) break;
  }

  std::cout << "total read size: " << reader.read_size() << "\n";

  return 0;
}