#pragma once

#include "../DiskHandler.hpp"
#include <fstream>
#include <memory>
#include <utility>
#include <vector>

class DiskReader : public DiskHandler {
  std::shared_ptr<std::istream> is_;
  std::uint64_t block_size_;

public:
  DiskReader();
  DiskReader(std::shared_ptr<std::istream> stream, std::uint64_t offset, std::uint64_t block_size);
  DiskReader(const DiskReader &disk_reader) = default;

  ~DiskReader() = default;
  auto operator=(const DiskReader &other) -> DiskReader & = default;
  DiskReader(DiskReader &&other) = default;
  auto operator=(DiskReader &&other) -> DiskReader & = default;

  [[nodiscard]] auto get_block_size() const noexcept -> std::uint64_t;
  auto set_block_size(std::uint64_t block_size) noexcept -> void;

  [[nodiscard]] auto read() const -> std::vector<std::byte>;
  auto read_next() -> std::vector<std::byte>;
};