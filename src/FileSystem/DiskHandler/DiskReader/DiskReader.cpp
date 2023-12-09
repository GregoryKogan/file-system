#include "DiskReader.hpp"

DiskReader::DiskReader(std::unique_ptr<std::istream> stream, std::uint64_t offset, std::uint64_t block_size)
    : DiskHandler(offset), is_(std::move(stream)), block_size_(block_size) {}

auto DiskReader::get_block_size() const noexcept -> std::uint64_t { return block_size_; }

void DiskReader::set_block_size(std::uint64_t block_size) noexcept { block_size_ = block_size; }

auto DiskReader::read_block() const -> std::vector<std::byte> {
  is_->seekg(static_cast<std::streamoff>(get_offset() + get_handled_size()));

  std::vector<std::byte> block;
  block.reserve(get_block_size());

  std::istreambuf_iterator<char> iter(*is_);
  std::istreambuf_iterator<char> end;

  for (std::size_t i = 0; i < get_block_size() && iter != end; ++i, ++iter) {
    block.push_back(static_cast<std::byte>(*iter));
  }

  return block;
}

auto DiskReader::read_next_block() -> std::vector<std::byte> {
  auto block = read_block();
  increase_handled_size(block.size());
  return block;
}
