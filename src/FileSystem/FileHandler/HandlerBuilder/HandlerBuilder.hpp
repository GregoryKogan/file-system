#pragma once

#include "../../DiskHandler/DiskReader/DiskReader.hpp"
#include "../../DiskHandler/DiskWriter/DiskWriter.hpp"
#include "../../FAT/FAT.hpp"
#include "../ByteReader/ClusterReader/ClusterReader.hpp"
#include "../ByteWriter/ClusterWriter/ClusterWriter.hpp"
#include "../FileReader/FileReader.hpp"
#include "../FileWriter/FileWriter.hpp"
#include "../MetadataHandler/MetadataHandler.hpp"

class HandlerBuilder {
  ClusterReader cluster_reader_;
  ClusterWriter cluster_writer_;
  FAT fat_;

public:
  HandlerBuilder();
  HandlerBuilder(DiskReader disk_reader, DiskWriter disk_writer, FAT fat, std::uint64_t clusters_start_offset,
                 std::uint64_t cluster_size);

  [[nodiscard]] auto build_byte_reader(std::uint64_t cluster) const -> ByteReader;
  [[nodiscard]] auto build_byte_writer(std::uint64_t cluster) const -> ByteWriter;
  [[nodiscard]] auto build_metadata_handler(std::uint64_t cluster) const -> MetadataHandler;
  [[nodiscard]] auto build_file_reader(std::uint64_t cluster) const -> FileReader;
  [[nodiscard]] auto build_file_writer(std::uint64_t cluster) const -> FileWriter;
};