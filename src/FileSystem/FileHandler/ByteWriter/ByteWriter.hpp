#pragma once

#include "../../FAT/FAT.hpp"
#include "ClusterWriter/ClusterWriter.hpp"

class ByteWriter {
  ClusterWriter cluster_writer_;
  FAT fat_;
  std::uint64_t cluster_;

public:
  ByteWriter(ClusterWriter cluster_writer, FAT fat, std::uint64_t cluster);
  ByteWriter(const ByteWriter &byte_writer) = default;

  auto operator=(const ByteWriter &other) -> ByteWriter = delete;
  ByteWriter(ByteWriter &&other) = default;
  auto operator=(ByteWriter &&other) -> ByteWriter = delete;
  ~ByteWriter() = default;

  auto write_bytes(std::uint64_t offset, const std::vector<std::byte> &bytes) -> std::uint64_t;
};