#include "FileReader.hpp"

FileReader::FileReader(ByteReader byte_reader, MetadataHandler metadata_handler, std::uint64_t offset,
                       std::uint64_t block_size)
    : FileHandler(std::move(metadata_handler), offset), byte_reader_(std::move(byte_reader)), block_size_(block_size) {}

auto FileReader::get_block_size() const noexcept -> std::uint64_t { return block_size_; }

auto FileReader::set_block_size(std::uint64_t block_size) noexcept -> void { block_size_ = block_size; }

auto FileReader::read() -> std::vector<std::byte> {
  auto meta = get_metadata_handler().read_metadata();

  auto size = std::min(block_size_, meta.get_size() - get_offset() - get_handled_size());
  auto offset = Metadata::get_metadata_size() + get_offset() + get_handled_size();

  return byte_reader_.read_bytes(offset, size);
}

auto FileReader::read_next() -> std::vector<std::byte> {
  auto block = read();
  increase_handled_size(block.size());
  return block;
}