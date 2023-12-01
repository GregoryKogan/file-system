#pragma once

#include "../../DiskHandler/DiskReader/DiskReader.hpp"
#include "../FileHandler.hpp"

class FileReader : public FileHandler {
  std::uint64_t block_size_;
  std::shared_ptr<DiskReader> disk_reader_;

public:
  struct BlockSize {
    std::uint64_t value;

    explicit BlockSize(std::uint64_t value);
  };

  static const std::uint64_t DEFAULT_BLOCK_SIZE = 1024;

  FileReader(FileData file_data, FileOffset offset, std::shared_ptr<FAT> fat, std::uint64_t cluster_size,
             std::shared_ptr<DiskReader> disk_reader, BlockSize block_size = BlockSize(DEFAULT_BLOCK_SIZE));

  [[nodiscard]] auto block_size() const noexcept -> std::uint64_t;

  void set_block_size(BlockSize block_size) noexcept;

  [[nodiscard]] auto read_block() const -> std::vector<std::byte>;
  auto read_next_block() -> std::vector<std::byte>;

private:
  [[nodiscard]] auto read_cluster(std::uint64_t cluster_index) const -> std::vector<std::byte>;
};