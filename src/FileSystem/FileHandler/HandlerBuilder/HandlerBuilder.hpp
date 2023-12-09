#pragma once

#include "../../DiskHandler/DiskReader/DiskReader.hpp"
#include "../../DiskHandler/DiskWriter/DiskWriter.hpp"
#include "../../FAT/FAT.hpp"
#include "../ByteReader/ClusterReader/ClusterReader.hpp"
#include "../ByteWriter/ClusterWriter/ClusterWriter.hpp"
#include "../MetadataHandler/MetadataHandler.hpp"

class HandlerBuilder {
  ClusterReader cluster_reader_;
  ClusterWriter cluster_writer_;
  FAT fat_;

public:
  HandlerBuilder(DiskReader disk_reader, DiskWriter disk_writer, FAT fat, std::uint64_t clusters_start_offset,
                 std::uint64_t cluster_size);

  auto build_metadata_handler(std::uint64_t cluster) -> MetadataHandler;
};