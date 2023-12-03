#pragma once

#include <fstream>
#include <memory>
#include <vector>

#include "../DiskHandler.hpp"

class DiskWriter : public DiskHandler {
  std::unique_ptr<std::ostream> os_;

public:
  DiskWriter(std::unique_ptr<std::ostream> stream, DiskOffset offset);

  void write(const std::vector<std::byte> &bytes) const;
  void write_next_block(const std::vector<std::byte> &bytes);
};