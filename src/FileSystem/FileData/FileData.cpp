#include "FileData.hpp"

FileData::FileData(std::string name, FileSize size, std::uint64_t first_cluster_index, bool is_directory)
    : name_(std::move(name)), size_(size.bytes), first_cluster_index_(first_cluster_index),
      is_directory_(is_directory) {}

auto FileData::name() const noexcept -> std::string { return name_; }

auto FileData::size() const noexcept -> FileSize { return FileSize{size_}; }

auto FileData::first_cluster_index() const noexcept -> std::uint64_t { return first_cluster_index_; }

auto FileData::is_directory() const noexcept -> bool { return is_directory_; }

auto FileData::set_name(std::string name) noexcept -> void { name_ = std::move(name); }

auto FileData::set_size(FileSize size) noexcept -> void { size_ = size.bytes; }

auto FileData::set_first_cluster_index(std::uint64_t first_cluster_index) noexcept -> void {
  first_cluster_index_ = first_cluster_index;
}

auto FileData::set_is_directory(bool is_directory) noexcept -> void { is_directory_ = is_directory; }

auto FileData::to_bytes() const -> std::vector<std::byte> {
  auto bytes = std::vector<std::byte>{};
  bytes.reserve(FILE_DATA_SIZE);

  auto name_bytes = Converter::to_bytes(name_, NAME_SIZE);
  bytes.insert(bytes.end(), name_bytes.begin(), name_bytes.end());

  auto size_bytes = Converter::to_bytes(size_);
  bytes.insert(bytes.end(), size_bytes.begin(), size_bytes.end());

  auto first_cluster_index_bytes = Converter::to_bytes(first_cluster_index_);
  bytes.insert(bytes.end(), first_cluster_index_bytes.begin(), first_cluster_index_bytes.end());

  auto is_directory_bytes = Converter::to_bytes(is_directory_);
  bytes.insert(bytes.end(), is_directory_bytes.begin(), is_directory_bytes.end());

  return bytes;
}

auto FileData::from_bytes(std::vector<std::byte> const &bytes) -> FileData {
  if (bytes.size() != FILE_DATA_SIZE) throw std::runtime_error("Invalid file data size");

  auto name_bytes = std::vector<std::byte>(bytes.begin(), bytes.begin() + NAME_SIZE);
  auto name = Converter::to_string(name_bytes);

  auto size_bytes = std::vector<std::byte>(bytes.begin() + NAME_SIZE, bytes.begin() + NAME_SIZE + SIZE_SIZE);
  auto size = Converter::to_uint64(size_bytes);

  auto first_cluster_index_bytes = std::vector<std::byte>(
      bytes.begin() + NAME_SIZE + SIZE_SIZE, bytes.begin() + NAME_SIZE + SIZE_SIZE + FIRST_CLUSTER_INDEX_SIZE);
  auto first_cluster_index = Converter::to_uint64(first_cluster_index_bytes);

  auto is_directory_bytes =
      std::vector<std::byte>(bytes.begin() + NAME_SIZE + SIZE_SIZE + FIRST_CLUSTER_INDEX_SIZE,
                             bytes.begin() + NAME_SIZE + SIZE_SIZE + FIRST_CLUSTER_INDEX_SIZE + IS_DIRECTORY_SIZE);

  auto is_directory = Converter::to_bool(is_directory_bytes);

  return FileData(name, FileSize{size}, first_cluster_index, is_directory);
}

auto FileData::to_string(bool verbose) const -> std::string {
  if (!verbose) {
    std::string is_directory_string = is_directory_ ? "/" : "";
    return name_ + is_directory_string;
  }

  std::string is_directory_string = is_directory_ ? "true" : "false";
  return "FileData { name: " + name_ + ", size: " + std::to_string(size_) +
         " bytes, first_cluster: " + std::to_string(first_cluster_index_) + ", is_dir: " + is_directory_string + " }";
}

auto FileData::file_data_size() noexcept -> std::uint64_t { return FILE_DATA_SIZE; }
