#include "FileReader.hpp"

FileReader::FileReader(std::uint64_t first_cluster, std::uint64_t offset, std::shared_ptr<FAT> fat,
                       std::uint64_t cluster_size, std::uint64_t clusters_start_offset,
                       std::shared_ptr<DiskReader> disk_reader, std::uint64_t block_size)
    : FileHandler(first_cluster, offset, std::move(fat), cluster_size), disk_reader_(std::move(disk_reader)),
      block_size_(block_size), clusters_start_offset_(clusters_start_offset) {}

auto FileReader::get_block_size() const noexcept -> std::uint64_t { return block_size_; }

void FileReader::set_block_size(std::uint64_t block_size) noexcept { block_size_ = block_size; }

auto FileReader::read_metadata() const -> Metadata {
  auto metadata_bytes = read_bytes(0, Metadata::get_metadata_size());
  return Metadata::from_bytes(metadata_bytes);
}

auto FileReader::read_block() const -> std::vector<std::byte> {
  auto meta = read_metadata();

  auto size = std::min(block_size_, meta.get_size() - get_offset() - get_handled_size());
  auto offset = Metadata::get_metadata_size() + get_offset() + get_handled_size();

  return read_bytes(offset, size);
}

auto FileReader::read_next_block() -> std::vector<std::byte> {
  auto block = read_block();
  increase_handled_size(block.size());
  return block;
}

auto FileReader::read_bytes(std::uint64_t offset, std::uint64_t size) const -> std::vector<std::byte> {
  std::vector<std::byte> bytes;
  bytes.reserve(size);

  auto cur_cluster = get_first_cluster();
  std::uint64_t file_position = 0;

  while (bytes.size() < size) {
    auto cluster_bytes = read_cluster(cur_cluster);
    for (auto byte : cluster_bytes) {
      if (file_position >= offset) { bytes.push_back(byte); }
      ++file_position;
      if (bytes.size() >= size) { break; }
    }

    if (bytes.size() < size) { cur_cluster = get_fat()->get_next(cur_cluster); }
  }

  return bytes;
}

auto FileReader::read_cluster(std::uint64_t cluster_index) const -> std::vector<std::byte> {
  auto cluster_offset = clusters_start_offset_ + cluster_index * get_cluster_size();
  disk_reader_->set_offset(cluster_offset);
  disk_reader_->set_block_size(get_cluster_size());
  return disk_reader_->read_block();
}