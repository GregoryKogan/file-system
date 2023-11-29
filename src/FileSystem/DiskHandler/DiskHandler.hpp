#pragma once

#include <utility>

class DiskHandler {
  std::uint64_t offset_;
  std::uint64_t handled_size_ = 0;

public:
  struct DiskOffset;

  explicit DiskHandler(DiskOffset offset);

  [[nodiscard]] auto offset() const noexcept -> std::uint64_t;
  [[nodiscard]] auto handled_size() const noexcept -> std::uint64_t;

  void set_offset(DiskOffset offset) noexcept;

protected:
  void increase_handled_size(std::uint64_t size) noexcept;
};

struct DiskHandler::DiskOffset {
  std::uint64_t value;

  explicit DiskOffset(std::uint64_t value);
};