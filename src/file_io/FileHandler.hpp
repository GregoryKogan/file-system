#pragma once

#include <utility>

class FileHandler {
  std::uint32_t offset_;
  std::uint32_t block_size_;
  std::uint32_t handled_size_ = 0;

public:
  struct FileOffset {
    std::uint32_t value;
  };

  struct BlockSize {
    std::uint32_t value;
  };

  FileHandler(FileOffset offset, BlockSize block_size);

  [[nodiscard]] auto offset() const noexcept -> std::uint32_t;
  [[nodiscard]] auto block_size() const noexcept -> std::uint32_t;
  [[nodiscard]] auto handled_size() const noexcept -> std::uint32_t;

  void set_offset(FileOffset offset) noexcept;
  void set_block_size(BlockSize block_size) noexcept;

protected:
  void increase_handled_size(std::uint32_t size) noexcept;
};