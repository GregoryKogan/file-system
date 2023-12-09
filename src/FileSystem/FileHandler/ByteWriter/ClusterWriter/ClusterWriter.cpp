#include "ClusterWriter.hpp"

#include <utility>

ClusterWriter::ClusterWriter(DiskWriter disk_writer, std::uint64_t clusters_start_offset, std::uint64_t cluster_size)
    : disk_writer_(std::move(disk_writer)), clusters_start_offset_(clusters_start_offset), cluster_size_(cluster_size) {
}

auto ClusterWriter::get_cluster_size() const -> std::uint64_t { return cluster_size_; }

auto ClusterWriter::write_at_position(std::uint64_t cluster_index, std::uint64_t position,
                                      const std::vector<std::byte> &bytes) -> std::uint64_t {
  std::uint64_t byte_to_write_count = std::min(static_cast<std::uint64_t>(bytes.size()), cluster_size_ - position);
  auto bytes_to_write =
      std::vector<std::byte>(bytes.begin(), bytes.begin() + static_cast<std::int64_t>(byte_to_write_count));

  auto position_offset = clusters_start_offset_ + cluster_index * cluster_size_ + position;
  disk_writer_.set_offset(position_offset);
  disk_writer_.write(bytes_to_write);

  return byte_to_write_count;
}

auto ClusterWriter::write_cluster(std::uint64_t cluster_index, const std::vector<std::byte> &bytes) -> std::uint64_t {
  return write_at_position(cluster_index, 0, bytes);
}