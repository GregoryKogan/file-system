#pragma once

#include <fstream>
#include <memory>
#include <vector>

#include "../DiskHandler.hpp"

class DiskReader : public DiskHandler {
  std::unique_ptr<std::istream> is_;
  std::uint64_t block_size_;

public:
  DiskReader(std::unique_ptr<std::istream> stream, std::uint64_t offset, std::uint64_t block_size);

  [[nodiscard]] auto get_block_size() const noexcept -> std::uint64_t;
  void set_block_size(std::uint64_t block_size) noexcept;

  [[nodiscard]] auto read_block() const -> std::vector<std::byte>;
  auto read_next_block() -> std::vector<std::byte>;
};