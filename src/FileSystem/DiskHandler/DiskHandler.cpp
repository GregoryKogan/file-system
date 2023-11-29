#include "DiskHandler.hpp"

DiskHandler::DiskHandler(DiskOffset offset) : offset_(offset.value) {}

auto DiskHandler::offset() const noexcept -> std::uint64_t { return offset_; }

auto DiskHandler::handled_size() const noexcept -> std::uint64_t { return handled_size_; }

void DiskHandler::set_offset(DiskOffset offset) noexcept {
  offset_ = offset.value;
  handled_size_ = 0;
}

void DiskHandler::increase_handled_size(std::uint64_t size) noexcept { handled_size_ += size; }

DiskHandler::DiskOffset::DiskOffset(std::uint64_t value) : value(value) {}
