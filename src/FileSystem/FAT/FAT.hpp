#pragma once

#include "../Converter/Converter.hpp"
#include "../DiskHandler/DiskReader/DiskReader.hpp"
#include "../DiskHandler/DiskWriter/DiskWriter.hpp"
#include <sstream>

class FAT {
  struct FATEntry;
  struct ClusterStatusOptions;

  static const std::uint64_t STATUS_SIZE = 1;
  static const std::uint64_t NEXT_CLUSTER_SIZE = 8;
  static const std::uint64_t ENTRY_SIZE = STATUS_SIZE + NEXT_CLUSTER_SIZE;
  static const std::uint64_t MAX_ENTRIES_TO_LOAD = 1000000;

  std::uint64_t entries_count_;
  DiskHandler::DiskOffset disk_offset_;

  std::shared_ptr<DiskReader> disk_reader_;
  std::shared_ptr<DiskWriter> disk_writer_;

public:
  explicit FAT(std::shared_ptr<DiskReader> &disk_reader_, std::shared_ptr<DiskWriter> &disk_writer_,
               DiskHandler::DiskOffset offset, std::uint64_t entries_count);

  [[nodiscard]] auto entries_count() const noexcept -> std::uint64_t;
  [[nodiscard]] auto entries() const -> std::vector<FATEntry>;

  static auto empty_entry_bytes() -> std::vector<std::byte>;
  static auto entry_size() -> std::uint64_t;
  static auto to_string(FAT const &fat) -> std::string;

private:
  static auto to_fat_entry(std::vector<std::byte> const &entry_bytes) -> FATEntry;
  static auto cluster_status_to_string(std::byte status) -> std::string;
};

struct FAT::FATEntry {
  std::byte status;
  std::uint64_t next_cluster;
};

struct FAT::ClusterStatusOptions {
  static const std::byte FREE = std::byte{238};
  static const std::byte BUSY = std::byte{187};
  static const std::byte LAST = std::byte{255};
};