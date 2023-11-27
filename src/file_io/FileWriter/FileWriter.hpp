#pragma once

#include <fstream>
#include <vector>

#include "../FileHandler.hpp"

class FileWriter : public FileHandler {
  std::unique_ptr<std::ostream> os_;

public:
  FileWriter(std::unique_ptr<std::ostream> stream, FileOffset offset);

  void write(const std::vector<std::byte> &bytes) const;
  void write_next_block(const std::vector<std::byte> &bytes);
};