#pragma once

#include <fstream>
#include <memory>
#include <vector>

#include "../DiskHandler.hpp"

class DiskReader : public DiskHandler {
  std::unique_ptr<std::istream> is_;
  std::uint64_t block_size_;

public:
  struct BlockSize {
    std::uint64_t value;

    explicit BlockSize(std::uint64_t value);
  };

  static const std::uint64_t DEFAULT_BLOCK_SIZE = 1024;

  DiskReader(std::unique_ptr<std::istream> stream, DiskOffset offset,
             BlockSize block_size = BlockSize(DEFAULT_BLOCK_SIZE));

  [[nodiscard]] auto block_size() const noexcept -> std::uint64_t;

  void set_block_size(BlockSize block_size) noexcept;

  [[nodiscard]] auto read_block() const -> std::vector<std::byte>;
  auto read_next_block() -> std::vector<std::byte>;
};