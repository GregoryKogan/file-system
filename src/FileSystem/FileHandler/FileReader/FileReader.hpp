#pragma once

#include "../../DiskHandler/DiskReader/DiskReader.hpp"
#include "../FileHandler.hpp"

class FileReader : public FileHandler {
  std::uint64_t block_size_;
  std::shared_ptr<DiskReader> disk_reader_;

  std::uint64_t clusters_start_offset_;

public:
  FileReader(std::uint64_t first_cluster, std::uint64_t offset, std::shared_ptr<FAT> fat, std::uint64_t cluster_size,
             std::uint64_t clusters_start_offset, std::shared_ptr<DiskReader> disk_reader, std::uint64_t block_size);

  [[nodiscard]] auto get_block_size() const noexcept -> std::uint64_t;
  void set_block_size(std::uint64_t block_size) noexcept;

  [[nodiscard]] auto read_metadata() const -> Metadata;
  [[nodiscard]] auto read_block() const -> std::vector<std::byte>;
  auto read_next_block() -> std::vector<std::byte>;

private:
  [[nodiscard]] auto read_bytes(std::uint64_t offset, std::uint64_t size) const -> std::vector<std::byte>;
  [[nodiscard]] auto read_cluster(std::uint64_t cluster_index) const -> std::vector<std::byte>;
};