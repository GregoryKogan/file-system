#pragma once

#include <optional>

#include "../Converter/Converter.hpp"
#include "../Metadata/Metadata.hpp"

class Directory {
  std::vector<std::uint64_t> child_clusters_;

public:
  Directory() = default;

  static auto from_bytes(std::vector<std::byte> const &bytes) -> Directory;
  [[nodiscard]] auto to_bytes() const -> std::vector<std::byte>;

  [[nodiscard]] auto list_files() const -> std::vector<std::uint64_t>;

  auto add_file(std::uint64_t cluster) -> void;
  auto remove_file(std::uint64_t cluster) -> void;
};