#pragma once

#include "../FileData/FileData.hpp"

class Directory {
  std::vector<FileData> files_;

public:
  explicit Directory(std::vector<FileData> files);

  static auto from_bytes(std::vector<std::byte> const &bytes) -> Directory;
  [[nodiscard]] auto to_bytes() const -> std::vector<std::byte>;

  [[nodiscard]] auto files() const -> std::vector<FileData>;

  [[nodiscard]] static auto make_root() -> Directory;
};