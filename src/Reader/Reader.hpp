#pragma once

#include <fstream>
#include <string>
#include <utility>
#include <vector>

struct FileOffset {
  std::uint32_t value;
};

struct ReadBlockSize {
  std::uint32_t value;
};

class Reader {
  std::unique_ptr<std::istream> is_;
  std::uint32_t offset_;
  std::uint32_t block_size_;
  std::uint32_t read_size_ = 0;

 public:
  Reader(std::unique_ptr<std::istream> stream, FileOffset offset,
         ReadBlockSize block_size);

  [[nodiscard]] auto offset() const noexcept -> FileOffset;
  [[nodiscard]] auto block_size() const noexcept -> ReadBlockSize;
  [[nodiscard]] auto read_size() const noexcept -> std::uint32_t;

  void set_offset(FileOffset offset) noexcept;
  void set_block_size(ReadBlockSize block_size) noexcept;

  [[nodiscard]] auto read_block() const -> std::vector<std::byte>;
  auto read_next_block() -> std::vector<std::byte>;
};