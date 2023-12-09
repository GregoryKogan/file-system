#pragma once

#include "../Converter/Converter.hpp"
#include <sstream>
#include <stdexcept>
#include <string>

class Metadata {
  static const std::uint64_t NAME_SIZE = 64;
  static const std::uint64_t SIZE_SIZE = 8;
  static const std::uint64_t FIRST_CLUSTER_SIZE = 8;
  static const std::uint64_t PARENT_FIRST_CLUSTER_SIZE = 8;
  static const std::uint64_t IS_DIRECTORY_SIZE = 1;

  std::string name_;
  std::uint64_t size_;
  std::uint64_t first_cluster_;
  std::uint64_t parent_first_cluster_;
  bool is_directory_;

public:
  Metadata(std::string name, std::uint64_t size, std::uint64_t first_cluster, std::uint64_t parent_first_cluster,
           bool is_directory);

  [[nodiscard]] auto get_name() const -> std::string;
  [[nodiscard]] auto get_size() const noexcept -> std::uint64_t;
  [[nodiscard]] auto get_first_cluster() const noexcept -> std::uint64_t;
  [[nodiscard]] auto get_parent_first_cluster() const noexcept -> std::uint64_t;
  [[nodiscard]] auto is_directory() const noexcept -> bool;

  auto set_name(std::string name) noexcept -> void;
  auto set_size(std::uint64_t size) noexcept -> void;
  auto set_first_cluster(std::uint64_t first_cluster) noexcept -> void;
  auto set_parent_first_cluster(std::uint64_t parent_first_cluster) noexcept -> void;
  auto set_is_directory(bool is_directory) noexcept -> void;

  [[nodiscard]] auto to_bytes() const -> std::vector<std::byte>;
  static auto from_bytes(std::vector<std::byte> const &bytes) -> Metadata;

  [[nodiscard]] static auto get_metadata_size() noexcept -> std::uint64_t;

  [[nodiscard]] auto static to_string(Metadata const &metadata) -> std::string;
};
