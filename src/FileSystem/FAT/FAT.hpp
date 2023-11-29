#pragma once

#include "../FSMaker/FSMaker.hpp"
#include <sstream>

class FAT {
public:
  struct FATEntry {
    std::byte status;
    std::uint64_t next_cluster;
  };

private:
  std::shared_ptr<DiskReader> fs_reader_;
  std::uint64_t entries_count_;

public:
  explicit FAT(std::shared_ptr<DiskReader> &fs_reader, FSMaker::Settings const &settings);

  [[nodiscard]] auto entries_count() const noexcept -> std::uint64_t;
  [[nodiscard]] auto entries() const -> std::vector<FATEntry>;

  static auto pretty_print_fat(FAT const &fat) -> std::string;

private:
  static const std::uint64_t MAX_ENTRIES_TO_STRINGIFY = 1000000;

  static auto to_fat_entry(std::vector<std::byte> const &entry_bytes) -> FATEntry;
  static auto to_string(std::byte status) -> std::string;
};