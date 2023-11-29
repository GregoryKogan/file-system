#pragma once

#include <utility>

class FileHandler {
  std::uint64_t offset_;
  std::uint64_t handled_size_ = 0;

public:
  struct FileOffset;

  explicit FileHandler(FileOffset offset);

  [[nodiscard]] auto offset() const noexcept -> std::uint64_t;
  [[nodiscard]] auto handled_size() const noexcept -> std::uint64_t;

  void set_offset(FileOffset offset) noexcept;

protected:
  void increase_handled_size(std::uint64_t size) noexcept;
};

struct FileHandler::FileOffset {
  std::uint64_t value;

  explicit FileOffset(std::uint64_t value);
};