#pragma once

#include "../DiskHandler.hpp"
#include <fstream>
#include <memory>
#include <utility>
#include <vector>

class DiskWriter : public DiskHandler {
  std::shared_ptr<std::ostream> os_;

public:
  DiskWriter();
  DiskWriter(std::shared_ptr<std::ostream> stream, std::uint64_t offset);
  DiskWriter(const DiskWriter &disk_writer) = default;

  ~DiskWriter() = default;
  auto operator=(const DiskWriter &other) -> DiskWriter & = default;
  DiskWriter(DiskWriter &&other) = default;
  auto operator=(DiskWriter &&other) -> DiskWriter & = default;

  void write(const std::vector<std::byte> &bytes) const;
  void write_next(const std::vector<std::byte> &bytes);
};