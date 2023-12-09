#pragma once

#include "../../FAT/FAT.hpp"
#include "ClusterReader/ClusterReader.hpp"

class ByteReader {
  ClusterReader cluster_reader_;
  FAT fat_;
  std::uint64_t cluster_;

public:
  ByteReader(ClusterReader cluster_reader, FAT fat, std::uint64_t cluster);
  ByteReader(const ByteReader &byte_reader) = default;

  auto operator=(const ByteReader &other) -> ByteReader = delete;
  ByteReader(ByteReader &&other) = default;
  auto operator=(ByteReader &&other) -> ByteReader = delete;
  ~ByteReader() = default;

  [[nodiscard]] auto read_bytes(std::uint64_t offset, std::uint64_t size) -> std::vector<std::byte>;
};