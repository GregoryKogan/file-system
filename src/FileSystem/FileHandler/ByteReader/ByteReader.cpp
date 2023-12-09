#include "ByteReader.hpp"

#include <utility>

ByteReader::ByteReader(ClusterReader cluster_reader, FAT fat, std::uint64_t cluster)
    : cluster_reader_(std::move(cluster_reader)), fat_(std::move(fat)), cluster_(cluster) {}

auto ByteReader::read_bytes(std::uint64_t offset, std::uint64_t size) -> std::vector<std::byte> {
  std::vector<std::byte> bytes;
  bytes.reserve(size);

  auto cur_cluster = cluster_;
  std::uint64_t file_position = 0;

  while (bytes.size() < size) {
    auto cluster_bytes = cluster_reader_.read_cluster(cur_cluster);
    for (auto byte : cluster_bytes) {
      if (file_position >= offset) { bytes.push_back(byte); }
      ++file_position;
      if (bytes.size() >= size) { break; }
    }

    if (bytes.size() < size) { cur_cluster = fat_.get_next(cur_cluster); }
  }

  return bytes;
}
