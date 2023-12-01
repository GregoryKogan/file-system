#pragma once

#include "../../DiskHandler/DiskWriter/DiskWriter.hpp"
#include "../FileHandler.hpp"
#include <iostream>

class FileWriter : public FileHandler {
  std::shared_ptr<DiskWriter> disk_writer_;

public:
  FileWriter(FileData file_data, FileOffset offset, std::shared_ptr<FAT> fat, std::uint64_t cluster_size,
             std::shared_ptr<DiskWriter> disk_writer);

  auto write_block(const std::vector<std::byte> &bytes) -> std::uint64_t;
  auto write_next_block(const std::vector<std::byte> &bytes) -> std::uint64_t;

private:
  void expand_file(std::uint64_t new_file_size);
};