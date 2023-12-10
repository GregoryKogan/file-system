#pragma once

#include "../ByteReader/ByteReader.hpp"
#include "../FileHandler.hpp"
#include <utility>

class FileReader : public FileHandler {
  ByteReader byte_reader_;
  std::uint64_t block_size_;

public:
  FileReader(ByteReader byte_reader, MetadataHandler metadata_handler, std::uint64_t offset, std::uint64_t block_size);

  [[nodiscard]] auto get_block_size() const noexcept -> std::uint64_t;
  void set_block_size(std::uint64_t block_size) noexcept;

  [[nodiscard]] auto read() -> std::vector<std::byte>;
  auto read_next() -> std::vector<std::byte>;
};