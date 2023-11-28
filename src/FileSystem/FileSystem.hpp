#pragma once

#include "../file_io/FileReader/FileReader.hpp"
#include "../file_io/FileWriter/FileWriter.hpp"
#include <cstdint>
#include <string>

#include <iostream>

class FileSystem {
  static const std::uint32_t MAX_BLOCK_SIZE = 1024;
  static const std::uint64_t BIG_THRESHOLD = 17179869184; // 16 GiB
  static const std::uint64_t MIN_FS_SIZE = 16;
  static const std::uint64_t MIN_CLUSTER_SIZE = 8;

public:
  struct Settings;

  static void make_fs(std::string const &path, Settings const &settings, bool allow_big = false);

private:
  static auto to_bytes(std::uint64_t value) -> std::vector<std::byte>;

  static void validate_settings(Settings const &settings, bool allow_big);

  static void fill_zeros(FileWriter &writer, std::uint64_t size);
  static void write_settings(FileWriter &writer, Settings const &settings);
};

struct FileSystem::Settings {
  std::uint64_t size;
  std::uint64_t cluster_size;
};