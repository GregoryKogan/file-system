#pragma once

#include <fstream>
#include <memory>
#include <vector>

#include "../DiskHandler.hpp"

class DiskWriter : public DiskHandler {
  std::shared_ptr<std::ostream> os_;

public:
  DiskWriter(std::shared_ptr<std::ostream> stream, std::uint64_t offset);
  DiskWriter(const DiskWriter &disk_writer) = default;

  ~DiskWriter() = default;
  auto operator=(const DiskWriter &other) -> DiskWriter = delete;
  DiskWriter(DiskWriter &&other) = default;
  auto operator=(DiskWriter &&other) -> DiskWriter = delete;

  void write(const std::vector<std::byte> &bytes) const;
  void write_next_block(const std::vector<std::byte> &bytes);
};