#pragma once

#include "../file_io/FileReader/FileReader.hpp"
#include "../file_io/FileWriter/FileWriter.hpp"
#include <cstdint>
#include <string>

#include <iostream>

class FileSystem {
public:
  struct Settings;

private:
  static const std::uint32_t MAX_BLOCK_SIZE = 1024;

  static auto to_bytes(std::uint64_t value) -> std::vector<std::byte>;

  static void fill_zeros(FileWriter &writer, std::uint64_t size);
  static void write_settings(FileWriter &writer, Settings const &settings);

public:
  static void make_fs(std::string const &path, Settings const &settings);
};

struct FileSystem::Settings {
  std::uint64_t size;
  std::uint64_t cluster_size;
};