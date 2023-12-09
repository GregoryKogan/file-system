#include "HandlerBuilder.hpp"

HandlerBuilder::HandlerBuilder(DiskReader disk_reader, DiskWriter disk_writer, FAT fat,
                               std::uint64_t clusters_start_offset, std::uint64_t cluster_size)
    : cluster_reader_(std::move(disk_reader), clusters_start_offset, cluster_size),
      cluster_writer_(std::move(disk_writer), clusters_start_offset, cluster_size), fat_(std::move(fat)) {}

auto HandlerBuilder::build_metadata_handler(std::uint64_t cluster) -> MetadataHandler {
  auto byte_reader = ByteReader(cluster_reader_, fat_, cluster);
  auto byte_writer = ByteWriter(cluster_writer_, fat_, cluster);
  return {byte_reader, byte_writer};
}