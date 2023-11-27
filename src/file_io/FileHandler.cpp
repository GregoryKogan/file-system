#include "FileHandler.hpp"

FileHandler::FileHandler(FileOffset offset, BlockSize block_size)
    : offset_(offset.value), block_size_(block_size.value) {}

auto FileHandler::offset() const noexcept -> std::uint32_t { return offset_; }

auto FileHandler::block_size() const noexcept -> std::uint32_t {
  return block_size_;
}

auto FileHandler::handled_size() const noexcept -> std::uint32_t {
  return handled_size_;
}

void FileHandler::set_offset(FileOffset offset) noexcept {
  offset_ = offset.value;
  handled_size_ = 0;
}

void FileHandler::set_block_size(BlockSize block_size) noexcept {
  block_size_ = block_size.value;
}

void FileHandler::increase_handled_size(std::uint32_t size) noexcept {
  handled_size_ += size;
}
