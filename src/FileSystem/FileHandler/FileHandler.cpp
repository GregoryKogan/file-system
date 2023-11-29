#include "FileHandler.hpp"

FileHandler::FileHandler(FileOffset offset) : offset_(offset.value) {}

auto FileHandler::offset() const noexcept -> std::uint64_t { return offset_; }

auto FileHandler::handled_size() const noexcept -> std::uint64_t { return handled_size_; }

void FileHandler::set_offset(FileOffset offset) noexcept {
  offset_ = offset.value;
  handled_size_ = 0;
}

void FileHandler::increase_handled_size(std::uint64_t size) noexcept { handled_size_ += size; }

FileHandler::FileOffset::FileOffset(std::uint64_t value) : value(value) {}
