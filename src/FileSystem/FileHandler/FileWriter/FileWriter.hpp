#pragma once

#include "../ByteWriter/ByteWriter.hpp"
#include "../FileHandler.hpp"
#include <utility>

class FileWriter : public FileHandler {
  ByteWriter byte_writer_;

public:
  FileWriter(ByteWriter byte_writer, MetadataHandler metadata_handler, std::uint64_t offset);

  auto write(const std::vector<std::byte> &bytes) -> void;
  auto write_next(const std::vector<std::byte> &bytes) -> void;
};