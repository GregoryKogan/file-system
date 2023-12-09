#include "Metadata.hpp"

Metadata::Metadata(std::string name, std::uint64_t size, std::uint64_t first_cluster,
                   std::uint64_t parent_first_cluster, bool is_directory)
    : name_(std::move(name)), size_(size), first_cluster_(first_cluster), parent_first_cluster_(parent_first_cluster),
      is_directory_(is_directory) {}

auto Metadata::get_name() const -> std::string { return name_; }

auto Metadata::get_size() const noexcept -> std::uint64_t { return size_; }

auto Metadata::get_first_cluster() const noexcept -> std::uint64_t { return first_cluster_; }

auto Metadata::get_parent_first_cluster() const noexcept -> std::uint64_t { return parent_first_cluster_; }

auto Metadata::is_directory() const noexcept -> bool { return is_directory_; }

auto Metadata::set_name(std::string name) noexcept -> void { name_ = std::move(name); }

auto Metadata::set_size(std::uint64_t size) noexcept -> void { size_ = size; }

auto Metadata::set_first_cluster(std::uint64_t first_cluster) noexcept -> void { first_cluster_ = first_cluster; }

auto Metadata::set_parent_first_cluster(std::uint64_t parent_first_cluster) noexcept -> void {
  parent_first_cluster_ = parent_first_cluster;
}

auto Metadata::set_is_directory(bool is_directory) noexcept -> void { is_directory_ = is_directory; }

auto Metadata::to_bytes() const -> std::vector<std::byte> {
  auto bytes = std::vector<std::byte>{};
  bytes.reserve(get_metadata_size());

  auto name_bytes = Converter::to_bytes(name_, NAME_SIZE);
  bytes.insert(bytes.end(), name_bytes.begin(), name_bytes.end());

  auto size_bytes = Converter::to_bytes(size_);
  bytes.insert(bytes.end(), size_bytes.begin(), size_bytes.end());

  auto first_cluster_bytes = Converter::to_bytes(first_cluster_);
  bytes.insert(bytes.end(), first_cluster_bytes.begin(), first_cluster_bytes.end());

  auto parent_first_cluster_bytes = Converter::to_bytes(parent_first_cluster_);
  bytes.insert(bytes.end(), parent_first_cluster_bytes.begin(), parent_first_cluster_bytes.end());

  auto is_directory_bytes = Converter::to_bytes(is_directory_);
  bytes.insert(bytes.end(), is_directory_bytes.begin(), is_directory_bytes.end());

  return bytes;
}

auto Metadata::from_bytes(std::vector<std::byte> const &bytes) -> Metadata {
  if (bytes.size() != get_metadata_size()) throw std::invalid_argument("Invalid metadata size");

  auto cur_offset = 0;

  auto name_bytes = std::vector<std::byte>(bytes.begin() + cur_offset, bytes.begin() + cur_offset + NAME_SIZE);
  auto name = Converter::to_string(name_bytes);
  cur_offset += NAME_SIZE;

  auto size_bytes = std::vector<std::byte>(bytes.begin() + cur_offset, bytes.begin() + cur_offset + SIZE_SIZE);
  auto size = Converter::to_uint64(size_bytes);
  cur_offset += SIZE_SIZE;

  auto first_cluster_bytes =
      std::vector<std::byte>(bytes.begin() + cur_offset, bytes.begin() + cur_offset + FIRST_CLUSTER_SIZE);
  auto first_cluster = Converter::to_uint64(first_cluster_bytes);
  cur_offset += FIRST_CLUSTER_SIZE;

  auto parent_first_cluster_bytes =
      std::vector<std::byte>(bytes.begin() + cur_offset, bytes.begin() + cur_offset + PARENT_FIRST_CLUSTER_SIZE);
  auto parent_first_cluster = Converter::to_uint64(parent_first_cluster_bytes);
  cur_offset += PARENT_FIRST_CLUSTER_SIZE;

  auto is_directory_bytes =
      std::vector<std::byte>(bytes.begin() + cur_offset, bytes.begin() + cur_offset + IS_DIRECTORY_SIZE);
  auto is_directory = Converter::to_bool(is_directory_bytes);

  return {name, size, first_cluster, parent_first_cluster, is_directory};
}

auto Metadata::get_metadata_size() noexcept -> std::uint64_t {
  return NAME_SIZE + SIZE_SIZE + FIRST_CLUSTER_SIZE + PARENT_FIRST_CLUSTER_SIZE + IS_DIRECTORY_SIZE;
}

auto Metadata::to_string(Metadata const &metadata, bool verbose) -> std::string {
  std::ostringstream oss;

  if (metadata.is_directory()) {
    oss << "D";
  } else {
    oss << "F";
  }
  oss << " " << metadata.get_name();

  if (verbose) {
    oss << ", " << metadata.get_size() << "b";
    oss << ", fc: " << metadata.get_first_cluster();
    oss << ", pfc: " << metadata.get_parent_first_cluster();
  }

  return oss.str();
}
