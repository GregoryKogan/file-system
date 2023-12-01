#include "FileReader.hpp"

FileReader::FileReader(FileData file_data, FileOffset offset, std::shared_ptr<FAT> fat, std::uint64_t cluster_size,
                       std::shared_ptr<DiskReader> disk_reader, BlockSize block_size)
    : FileHandler(std::move(file_data), offset, std::move(fat), cluster_size), disk_reader_(std::move(disk_reader)),
      block_size_(block_size.value) {}

auto FileReader::block_size() const noexcept -> std::uint64_t { return block_size_; }

void FileReader::set_block_size(BlockSize block_size) noexcept { block_size_ = block_size.value; }

auto FileReader::read_block() const -> std::vector<std::byte> {
  auto out_size = std::min(block_size_, file_data().size().bytes - offset() - handled_size());

  std::vector<std::byte> block;
  block.reserve(out_size);

  auto cur_cluster = file_data().first_cluster_index();
  std::uint64_t file_position = 0;

  while (block.size() < out_size) {
    auto cluster_bytes = read_cluster(cur_cluster);
    for (auto byte : cluster_bytes) {
      if (file_position >= offset() + handled_size()) { block.push_back(byte); }
      ++file_position;
      if (block.size() >= out_size) { break; }
    }

    if (block.size() < out_size) { cur_cluster = fat()->get_next(cur_cluster); }
  }

  return block;
}

auto FileReader::read_next_block() -> std::vector<std::byte> {
  auto block = read_block();
  increase_handled_size(block.size());
  return block;
}

auto FileReader::read_cluster(std::uint64_t cluster_index) const -> std::vector<std::byte> {
  auto cluster_offset = fat()->cluster_offset(cluster_index, cluster_size());
  disk_reader_->set_offset(cluster_offset);
  disk_reader_->set_block_size(DiskReader::BlockSize(cluster_size()));
  return disk_reader_->read_block();
}

FileReader::BlockSize::BlockSize(std::uint64_t value) : value(value) {}