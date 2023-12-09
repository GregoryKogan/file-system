#pragma once

#include <cstdint>
#include <utility>

class DiskHandler {
  std::uint64_t offset_;
  std::uint64_t handled_size_ = 0;

public:
  explicit DiskHandler(std::uint64_t offset);

  [[nodiscard]] auto get_offset() const noexcept -> std::uint64_t;
  [[nodiscard]] auto get_handled_size() const noexcept -> std::uint64_t;

  void set_offset(std::uint64_t offset) noexcept;

protected:
  void increase_handled_size(std::uint64_t size) noexcept;
};
