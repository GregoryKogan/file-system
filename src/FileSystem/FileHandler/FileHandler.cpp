#include "FileHandler.hpp"

FileHandler::FileHandler(FileData file_data, FileOffset offset, std::shared_ptr<FAT> fat, std::uint64_t cluster_size)
    : fat_(std::move(fat)), cluster_size_(cluster_size), file_data_(std::move(file_data)), offset_(offset.value) {}

auto FileHandler::offset() const noexcept -> std::uint64_t { return offset_; }

auto FileHandler::handled_size() const noexcept -> std::uint64_t { return handled_size_; }

void FileHandler::set_offset(FileOffset offset) {
  if (offset.value > file_data_.size().bytes) { throw std::runtime_error("Offset is out of file range"); }

  offset_ = offset.value;
  handled_size_ = 0;
}

auto FileHandler::fat() const noexcept -> std::shared_ptr<FAT> { return fat_; }

auto FileHandler::cluster_size() const noexcept -> std::uint64_t { return cluster_size_; }

auto FileHandler::file_data() const noexcept -> FileData { return file_data_; }

void FileHandler::increase_handled_size(std::uint64_t size) noexcept { handled_size_ += size; }

FileHandler::FileOffset::FileOffset(std::uint64_t value) : value(value) {}
