#include "Directory.hpp"

auto Directory::from_bytes(std::vector<std::byte> const &bytes) -> Directory {
  auto directory = Directory();

  auto step = static_cast<int>(Converter::get_uint64_size());
  auto cur_offset = 0;
  while (cur_offset < bytes.size()) {
    auto cluster_bytes = std::vector<std::byte>(bytes.begin() + cur_offset, bytes.begin() + cur_offset + step);
    auto cluster = Converter::to_uint64(cluster_bytes);
    cur_offset += step;

    directory.add_file(cluster);
  }

  return directory;
}

auto Directory::to_bytes() const -> std::vector<std::byte> {
  auto bytes = std::vector<std::byte>{};
  bytes.reserve(child_clusters_.size() * Converter::get_uint64_size());

  for (auto cluster : child_clusters_) {
    auto cluster_bytes = Converter::to_bytes(cluster);
    bytes.insert(bytes.end(), cluster_bytes.begin(), cluster_bytes.end());
  }

  return bytes;
}

auto Directory::list_files() const -> std::vector<std::uint64_t> { return child_clusters_; }

auto Directory::add_file(std::uint64_t cluster) -> void { child_clusters_.push_back(cluster); }

auto Directory::remove_file(std::uint64_t cluster) -> void {
  auto file_it = std::find(child_clusters_.begin(), child_clusters_.end(), cluster);
  if (file_it != child_clusters_.end()) child_clusters_.erase(file_it);
}
