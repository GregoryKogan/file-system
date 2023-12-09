#include "FileHandler.hpp"

FileHandler::FileHandler(std::uint64_t first_cluster, std::uint64_t offset, std::shared_ptr<FAT> fat,
                         std::uint64_t cluster_size)
    : first_cluster_(first_cluster), fat_(std::move(fat)), cluster_size_(cluster_size), offset_(offset) {}

auto FileHandler::get_offset() const noexcept -> std::uint64_t { return offset_; }

void FileHandler::set_offset(std::uint64_t offset) {
  offset_ = offset;
  handled_size_ = 0;
}

auto FileHandler::get_handled_size() const noexcept -> std::uint64_t { return handled_size_; }

auto FileHandler::get_first_cluster() const noexcept -> std::uint64_t { return first_cluster_; }

auto FileHandler::get_fat() const noexcept -> std::shared_ptr<FAT> { return fat_; }

auto FileHandler::get_cluster_size() const noexcept -> std::uint64_t { return cluster_size_; }

void FileHandler::increase_handled_size(std::uint64_t size) noexcept { handled_size_ += size; }
