#pragma once

#include "../FAT/FAT.hpp"
#include "../Metadata/Metadata.hpp"

class FileHandler {
  std::uint64_t first_cluster_;

  std::shared_ptr<FAT> fat_;
  std::uint64_t cluster_size_;

  std::uint64_t offset_;
  std::uint64_t handled_size_ = 0;

public:
  FileHandler(std::uint64_t first_cluster, std::uint64_t offset, std::shared_ptr<FAT> fat, std::uint64_t cluster_size);

  [[nodiscard]] auto get_offset() const noexcept -> std::uint64_t;
  void set_offset(std::uint64_t offset);

  [[nodiscard]] auto get_handled_size() const noexcept -> std::uint64_t;

protected:
  [[nodiscard]] auto get_first_cluster() const noexcept -> std::uint64_t;
  [[nodiscard]] auto get_fat() const noexcept -> std::shared_ptr<FAT>;
  [[nodiscard]] auto get_cluster_size() const noexcept -> std::uint64_t;

  void increase_handled_size(std::uint64_t size) noexcept;
};