#include "FileReader.hpp"

FileReader::FileReader(std::unique_ptr<std::istream> stream, FileOffset offset, BlockSize block_size)
    : FileHandler(offset), is_(std::move(stream)), block_size_(block_size.value) {}

auto FileReader::block_size() const noexcept -> std::uint64_t { return block_size_; }

void FileReader::set_block_size(BlockSize block_size) noexcept { block_size_ = block_size.value; }

auto FileReader::read_block() const -> std::vector<std::byte> {
  is_->seekg(static_cast<std::streamoff>(offset() + handled_size()));

  std::vector<std::byte> block;
  block.reserve(block_size());

  std::istreambuf_iterator<char> iter(*is_);
  std::istreambuf_iterator<char> end;

  for (std::size_t i = 0; i < block_size() && iter != end; ++i, ++iter) {
    block.push_back(static_cast<std::byte>(*iter));
  }

  return block;
}

auto FileReader::read_next_block() -> std::vector<std::byte> {
  auto block = read_block();
  increase_handled_size(block.size());
  return block;
}

FileReader::BlockSize::BlockSize(std::uint64_t value) : value(value) {}
