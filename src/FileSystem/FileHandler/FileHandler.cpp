#include "FileHandler.hpp"

FileHandler::FileHandler(MetadataHandler metadata_handler, std::uint64_t offset)
    : metadata_handler_(std::move(metadata_handler)), offset_(offset) {}

auto FileHandler::get_offset() const noexcept -> std::uint64_t { return offset_; }

auto FileHandler::set_offset(std::uint64_t offset) -> void {
  offset_ = offset;
  handled_size_ = 0;
}

auto FileHandler::get_handled_size() const noexcept -> std::uint64_t { return handled_size_; }

auto FileHandler::increase_handled_size(std::uint64_t size) noexcept -> void { handled_size_ += size; }

auto FileHandler::get_metadata_handler() const noexcept -> MetadataHandler { return metadata_handler_; }
