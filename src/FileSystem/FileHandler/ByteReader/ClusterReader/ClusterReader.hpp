#pragma once

#include "../../../DiskHandler/DiskReader/DiskReader.hpp"

class ClusterReader {
  DiskReader disk_reader_;
  std::uint64_t clusters_start_offset_;
  std::uint64_t cluster_size_;

public:
  ClusterReader(DiskReader disk_reader, std::uint64_t clusters_start_offset, std::uint64_t cluster_size);
  ClusterReader(const ClusterReader &cluster_reader) = default;

  ~ClusterReader() = default;
  auto operator=(const ClusterReader &other) -> ClusterReader & = default;
  ClusterReader(ClusterReader &&other) = default;
  auto operator=(ClusterReader &&other) -> ClusterReader & = default;

  [[nodiscard]] auto read_cluster(std::uint64_t cluster_index) -> std::vector<std::byte>;
};