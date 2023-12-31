#pragma once

#include "MetadataHandler/MetadataHandler.hpp"

class FileHandler {
  MetadataHandler metadata_handler_;
  std::uint64_t offset_;
  std::uint64_t handled_size_ = 0;

public:
  FileHandler(MetadataHandler metadata_handler, std::uint64_t offset);

  [[nodiscard]] auto get_offset() const noexcept -> std::uint64_t;
  auto set_offset(std::uint64_t offset) -> void;

  [[nodiscard]] auto get_handled_size() const noexcept -> std::uint64_t;

protected:
  auto increase_handled_size(std::uint64_t size) noexcept -> void;

  [[nodiscard]] auto get_metadata_handler() const noexcept -> MetadataHandler;
};