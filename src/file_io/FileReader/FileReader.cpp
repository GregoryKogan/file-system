#include "FileReader.hpp"

FileReader::FileReader(std::unique_ptr<std::istream> stream, FileOffset offset, BlockSize block_size)
    : FileHandler(offset, block_size), is_(std::move(stream)) {}

auto FileReader::read_block() const -> std::vector<std::byte> {
  is_->seekg(offset() + handled_size());
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
