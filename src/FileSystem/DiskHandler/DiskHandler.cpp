#include "DiskHandler.hpp"

DiskHandler::DiskHandler(std::uint64_t offset) : offset_(offset) {}

auto DiskHandler::get_offset() const noexcept -> std::uint64_t { return offset_; }

auto DiskHandler::get_handled_size() const noexcept -> std::uint64_t { return handled_size_; }

void DiskHandler::set_offset(std::uint64_t offset) noexcept {
  offset_ = offset;
  handled_size_ = 0;
}

void DiskHandler::increase_handled_size(std::uint64_t size) noexcept { handled_size_ += size; }