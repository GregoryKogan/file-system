#include "ByteWriter.hpp"

ByteWriter::ByteWriter(ClusterWriter cluster_writer, FAT fat, std::uint64_t cluster)
    : cluster_writer_(std::move(cluster_writer)), fat_(std::move(fat)), cluster_(cluster) {}

auto ByteWriter::write_bytes(std::uint64_t offset, const std::vector<std::byte> &bytes) -> std::uint64_t {
  std::uint64_t bytes_written = 0;
  std::uint64_t file_position = 0;
  auto cluster_size = cluster_writer_.get_cluster_size();

  auto cur_cluster = cluster_;
  while (bytes_written < bytes.size()) {
    if (file_position + cluster_size < offset) {
      file_position += cluster_size;
    } else {
      std::uint64_t position = file_position < offset ? offset - file_position : 0;
      auto bytes_to_write =
          std::vector<std::byte>(bytes.begin() + static_cast<std::int64_t>(bytes_written), bytes.end());
      bytes_written += cluster_writer_.write_at_position(cur_cluster, position, bytes_to_write);

      file_position += position + bytes_written;
    }

    if (bytes_written < bytes.size()) {
      if (fat_.is_last(cur_cluster)) {
        cur_cluster = fat_.allocate_next(cur_cluster);
      } else {
        cur_cluster = fat_.get_next(cur_cluster);
      }
    }
  }

  return file_position;
}
