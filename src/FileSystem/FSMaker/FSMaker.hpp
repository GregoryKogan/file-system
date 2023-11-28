#pragma once

#include "../../file_io/FileReader/FileReader.hpp"
#include "../../file_io/FileWriter/FileWriter.hpp"
#include <cstdint>
#include <string>

class FSMaker {
  static const std::uint32_t MAX_BLOCK_SIZE = 1024;
  static const std::uint64_t BIG_THRESHOLD = 17179869184; // 16 GiB
  static const std::uint64_t MIN_FS_SIZE = 16;
  static const std::uint64_t MIN_CLUSTER_SIZE = 8;

  static const std::uint64_t SETTINGS_SIZE = 16;

public:
  struct Settings;

  static const std::uint64_t FAT_STATUS_SIZE = 1;
  static const std::uint64_t FAT_NEXT_CLUSTER_SIZE = 8;
  static const std::uint64_t FAT_ENTRY_SIZE = FAT_STATUS_SIZE + FAT_NEXT_CLUSTER_SIZE;

  static const struct ClusterStatus {
    static const std::byte FREE = std::byte{238};
    static const std::byte BUSY = std::byte{187};
    static const std::byte LAST = std::byte{255};
  } ClusterStatus;

  static void make_fs(std::string const &path, Settings const &settings, bool allow_big = false);

  static auto calculate_fat_entries_count(Settings const &settings) -> std::uint64_t;

private:
  static auto to_bytes(std::uint64_t value) -> std::vector<std::byte>;

  static void validate_settings(Settings const &settings, bool allow_big);

  static void fill_zeros(FileWriter &writer, std::uint64_t size);
  static void write_settings(FileWriter &writer, Settings const &settings);

  static void write_fat(FileWriter &writer, Settings const &settings);
};

struct FSMaker::Settings {
  std::uint64_t size;
  std::uint64_t cluster_size;
};