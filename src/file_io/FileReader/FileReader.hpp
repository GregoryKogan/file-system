#pragma once

#include <fstream>
#include <vector>

#include "../FileHandler.hpp"

class FileReader : public FileHandler {
  std::unique_ptr<std::istream> is_;

 public:
  FileReader(std::unique_ptr<std::istream> stream, FileOffset offset,
             BlockSize block_size);

  [[nodiscard]] auto read_block() const -> std::vector<std::byte>;
  auto read_next_block() -> std::vector<std::byte>;
};