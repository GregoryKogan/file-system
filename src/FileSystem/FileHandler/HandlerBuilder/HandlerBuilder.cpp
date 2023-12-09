#include "HandlerBuilder.hpp"

HandlerBuilder::HandlerBuilder() : {
  auto disk_reader = DiskReader();
  auto disk_writer = DiskWriter();
  cluster_reader_ = ClusterReader(disk_reader, 0, 0);
  cluster_writer_ = ClusterWriter(disk_writer, 0, 0);
}

HandlerBuilder::HandlerBuilder(DiskReader disk_reader, DiskWriter disk_writer, FAT fat,
                               std::uint64_t clusters_start_offset, std::uint64_t cluster_size)
    : cluster_reader_(std::move(disk_reader), clusters_start_offset, cluster_size),
      cluster_writer_(std::move(disk_writer), clusters_start_offset, cluster_size), fat_(std::move(fat)) {}

auto HandlerBuilder::build_byte_reader(std::uint64_t cluster) const -> ByteReader {
  return {cluster_reader_, fat_, cluster};
}

auto HandlerBuilder::build_byte_writer(std::uint64_t cluster) const -> ByteWriter {
  return {cluster_writer_, fat_, cluster};
}

auto HandlerBuilder::build_metadata_handler(std::uint64_t cluster) const -> MetadataHandler {
  return {build_byte_reader(cluster), build_byte_writer(cluster)};
}

auto HandlerBuilder::build_file_reader(std::uint64_t cluster) const -> FileReader {
  return {build_byte_reader(cluster), build_metadata_handler(cluster), 0, 0};
}

auto HandlerBuilder::build_file_writer(std::uint64_t cluster) const -> FileWriter {
  return {build_byte_writer(cluster), build_metadata_handler(cluster), 0};
}
