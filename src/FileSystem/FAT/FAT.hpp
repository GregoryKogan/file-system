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
  std::uint64_t disk_offset_;

  DiskReader disk_reader_;
  DiskWriter disk_writer_;

public:
  FAT();
  FAT(DiskReader disk_reader_, DiskWriter disk_writer_, std::uint64_t offset, std::uint64_t entries_count);

  [[nodiscard]] auto entries_count() const noexcept -> std::uint64_t;

  [[nodiscard]] auto allocate() -> std::uint64_t;
  [[nodiscard]] auto allocate_next(std::uint64_t cluster_index) -> std::uint64_t;
  auto free(std::uint64_t cluster_index) -> void;
  auto set_next(std::uint64_t cluster_index, std::uint64_t next_cluster_index) -> void;
  [[nodiscard]] auto get_next(std::uint64_t cluster_index) -> std::uint64_t;
  [[nodiscard]] auto is_last(std::uint64_t cluster_index) -> bool;
  [[nodiscard]] auto is_allocated(std::uint64_t cluster_index) -> bool;

  static auto empty_entry_bytes() -> std::vector<std::byte>;
  static auto entry_size() -> std::uint64_t;
  static auto to_string(FAT const &fat) -> std::string;

private:
  [[nodiscard]] auto entries() -> std::vector<FATEntry>;
  [[nodiscard]] auto get_entry(std::uint64_t cluster_index) -> FATEntry;
  auto set_entry(std::uint64_t cluster_index, FATEntry const &entry) -> void;

  static auto to_fat_entry(std::vector<std::byte> const &entry_bytes) -> FATEntry;
  static auto to_bytes(FATEntry const &entry) -> std::vector<std::byte>;
  static auto cluster_status_to_string(std::byte status) -> std::string;
};

struct FAT::FATEntry {
  std::byte status;
  std::uint64_t next_cluster;
};

struct FAT::ClusterStatusOptions {
  static const std::byte FREE = std::byte{255};
  static const std::byte ALLOCATED = std::byte{170};
  static const std::byte LAST = std::byte{238};
};