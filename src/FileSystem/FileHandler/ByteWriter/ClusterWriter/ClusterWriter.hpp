#pragma once

#include "../../../DiskHandler/DiskWriter/DiskWriter.hpp"

class ClusterWriter {
  DiskWriter disk_writer_;
  std::uint64_t clusters_start_offset_;
  std::uint64_t cluster_size_;

public:
  ClusterWriter(DiskWriter disk_writer, std::uint64_t clusters_start_offset, std::uint64_t cluster_size);

  [[nodiscard]] auto get_cluster_size() const -> std::uint64_t;

  auto write_at_position(std::uint64_t cluster_index, std::uint64_t position, const std::vector<std::byte> &bytes)
      -> std::uint64_t;
  auto write_cluster(std::uint64_t cluster_index, const std::vector<std::byte> &bytes) -> std::uint64_t;
};