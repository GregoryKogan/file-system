#include "ByteWriter.hpp"

ByteWriter::ByteWriter(ClusterWriter cluster_writer, FAT fat, std::uint64_t cluster)
    : cluster_writer_(std::move(cluster_writer)), fat_(std::move(fat)), cluster_(cluster) {}

auto ByteWriter::write_bytes(std::uint64_t offset, const std::vector<std::byte> &bytes) -> std::uint64_t {
  auto cluster_number = offset / cluster_writer_.get_cluster_size();

  auto cur_cluster = cluster_;
  for (std::uint64_t i = 0; i < cluster_number; ++i) {
    if (fat_.is_last(cur_cluster)) {
      cur_cluster = fat_.allocate_next(cur_cluster);
    } else {
      cur_cluster = fat_.get_next(cur_cluster);
    }
  }

  auto bytes_written = std::uint64_t{0};
  auto position = offset % cluster_writer_.get_cluster_size();
  auto bytes_to_write = bytes;

  while (bytes_written < bytes.size()) {
    auto bytes_written_in_cluster = cluster_writer_.write_at_position(cur_cluster, position, bytes_to_write);
    bytes_written += bytes_written_in_cluster;
    position = 0;
    bytes_to_write = std::vector<std::byte>(
        bytes_to_write.begin() + static_cast<std::int64_t>(bytes_written_in_cluster), bytes_to_write.end());

    if (bytes_written < bytes.size()) {
      if (fat_.is_last(cur_cluster)) {
        cur_cluster = fat_.allocate_next(cur_cluster);
      } else {
        cur_cluster = fat_.get_next(cur_cluster);
      }
    }
  }

  return bytes_written + offset;
}
