#pragma once

#include "../Converter/Converter.hpp"
#include <string>

class FileData {
  static const std::uint64_t NAME_SIZE = 64;
  static const std::uint64_t SIZE_SIZE = 8;
  static const std::uint64_t FIRST_CLUSTER_INDEX_SIZE = 8;
  static const std::uint64_t IS_DIRECTORY_SIZE = 1;
  static const std::uint64_t FILE_DATA_SIZE = NAME_SIZE + SIZE_SIZE + FIRST_CLUSTER_INDEX_SIZE + IS_DIRECTORY_SIZE;

  std::string name_;
  std::uint64_t size_;
  std::uint64_t first_cluster_index_;
  bool is_directory_;

public:
  struct FileSize {
    std::uint64_t bytes;
  };

  FileData(std::string name, FileSize size, std::uint64_t first_cluster_index, bool is_directory);

  [[nodiscard]] auto name() const noexcept -> std::string;
  [[nodiscard]] auto size() const noexcept -> FileSize;
  [[nodiscard]] auto first_cluster_index() const noexcept -> std::uint64_t;
  [[nodiscard]] auto is_directory() const noexcept -> bool;

  [[nodiscard]] auto to_bytes() const -> std::vector<std::byte>;
  static auto from_bytes(std::vector<std::byte> const &bytes) -> FileData;
};
