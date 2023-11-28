#pragma once

#include <fstream>
#include <vector>

#include "../FileHandler.hpp"

class FileReader : public FileHandler {
  std::unique_ptr<std::istream> is_;
  std::uint64_t block_size_;

public:
  struct BlockSize;

  FileReader(std::unique_ptr<std::istream> stream, FileOffset offset, BlockSize block_size);

  [[nodiscard]] auto block_size() const noexcept -> std::uint64_t;

  void set_block_size(BlockSize block_size) noexcept;

  [[nodiscard]] auto read_block() const -> std::vector<std::byte>;
  auto read_next_block() -> std::vector<std::byte>;
};

struct FileReader::BlockSize {
  std::uint64_t value;

  explicit BlockSize(std::uint64_t value);
};