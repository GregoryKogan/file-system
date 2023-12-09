#include "ClusterReader.hpp"

#include <utility>

ClusterReader::ClusterReader(DiskReader disk_reader, std::uint64_t clusters_start_offset, std::uint64_t cluster_size)
    : disk_reader_(std::move(disk_reader)), clusters_start_offset_(clusters_start_offset), cluster_size_(cluster_size) {
}

auto ClusterReader::read_cluster(std::uint64_t cluster_index) -> std::vector<std::byte> {
  auto cluster_offset = clusters_start_offset_ + cluster_index * cluster_size_;
  disk_reader_.set_offset(cluster_offset);
  disk_reader_.set_block_size(cluster_size_);
  return disk_reader_.read_block();
}
