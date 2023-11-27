#include "Reader.hpp"

Reader::Reader(std::unique_ptr<std::istream> stream, FileOffset offset,
               ReadBlockSize block_size)
    : is_(std::move(stream)),
      offset_(offset.value),
      block_size_(block_size.value) {}

auto Reader::offset() const noexcept -> FileOffset {
  return FileOffset{offset_};
}

auto Reader::block_size() const noexcept -> ReadBlockSize {
  return ReadBlockSize{block_size_};
}

auto Reader::read_size() const noexcept -> std::uint32_t { return read_size_; }

void Reader::set_offset(FileOffset offset) noexcept { offset_ = offset.value; }

void Reader::set_block_size(ReadBlockSize block_size) noexcept {
  block_size_ = block_size.value;
}

auto Reader::read_block() const -> std::vector<std::byte> {
  is_->seekg(offset_ + read_size_);
  std::vector<std::byte> block;
  block.reserve(block_size_);

  std::istreambuf_iterator<char> iter(*is_);
  std::istreambuf_iterator<char> end;

  for (std::size_t i = 0; i < block_size_ && iter != end; ++i, ++iter) {
    block.push_back(static_cast<std::byte>(*iter));
  }

  return block;
}

auto Reader::read_next_block() -> std::vector<std::byte> {
  auto block = read_block();
  read_size_ += block.size();
  return block;
}
