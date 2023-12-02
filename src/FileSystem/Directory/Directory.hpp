#pragma once

#include "../FileData/FileData.hpp"
#include <iostream>
#include <optional>

class Directory {
  std::vector<FileData> files_;

public:
  explicit Directory(std::vector<FileData> files);

  static auto from_bytes(std::vector<std::byte> const &bytes) -> Directory;
  [[nodiscard]] auto to_bytes() const -> std::vector<std::byte>;

  [[nodiscard]] auto files() const -> std::vector<FileData>;
  [[nodiscard]] auto find(std::string const &name) const -> std::optional<FileData>;

  [[nodiscard]] static auto make_root() -> Directory;
};