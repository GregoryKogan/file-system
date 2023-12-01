#pragma once

#include "../Converter/Converter.hpp"
#include "../Directory/Directory.hpp"
#include "../DiskHandler/DiskReader/DiskReader.hpp"
#include "../DiskHandler/DiskWriter/DiskWriter.hpp"
#include "../FAT/FAT.hpp"
#include <cstdint>
#include <string>

class FSMaker {
  static const std::uint32_t MAX_BLOCK_SIZE = 1024;
  static const std::uint64_t BIG_THRESHOLD = 17179869184; // 16 GiB
  static const std::uint64_t MIN_FS_SIZE = 16;
  static const std::uint64_t MIN_CLUSTER_SIZE = 8;

  static const std::uint64_t SETTINGS_SIZE = 16;
  static const std::uint64_t FAT_OFFSET = SETTINGS_SIZE;

public:
  struct Settings {
    std::uint64_t size;
    std::uint64_t cluster_size;
  };

  static void make_fs(std::string const &path, Settings const &settings, bool allow_big = false);
  static auto fat_offset() -> std::uint64_t;
  static auto calculate_fat_entries_count(Settings const &settings) -> std::uint64_t;

private:
  static void validate_settings(Settings const &settings, bool allow_big);

  static void fill_zeros(DiskWriter &writer, std::uint64_t size);
  static void write_settings(DiskWriter &writer, Settings const &settings);
  static void write_fat(DiskWriter &writer, Settings const &settings);
};