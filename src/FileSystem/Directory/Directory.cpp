#include "Directory.hpp"

Directory::Directory(std::vector<FileData> files) : files_(std::move(files)) {}

auto Directory::from_bytes(std::vector<std::byte> const &bytes) -> Directory {
  auto file_data_size = FileData::file_data_size();
  if (bytes.size() % file_data_size != 0) { throw std::runtime_error("Invalid directory size"); }

  auto file_count = bytes.size() / file_data_size;
  auto files = std::vector<FileData>{};
  files.reserve(file_count);

  for (auto i = 0; i < file_count; i++) {
    auto file_bytes = std::vector<std::byte>(bytes.begin() + static_cast<int>(i * file_data_size),
                                             bytes.begin() + static_cast<int>((i + 1) * file_data_size));
    files.push_back(FileData::from_bytes(file_bytes));
  }

  return Directory(files);
}

auto Directory::to_bytes() const -> std::vector<std::byte> {
  auto bytes = std::vector<std::byte>{};
  bytes.reserve(files_.size() * FileData::file_data_size());

  for (auto const &file : files_) {
    auto file_bytes = file.to_bytes();
    bytes.insert(bytes.end(), file_bytes.begin(), file_bytes.end());
  }

  return bytes;
}

auto Directory::files() const -> std::vector<FileData> { return files_; }

auto Directory::find(std::string const &name) const -> std::optional<FileData> {
  for (auto const &file : files_) {
    if (file.name() == name) { return file; }
  }
  return {};
}

auto Directory::add_file(FileData const &file) -> void {
  if (find(file.name()).has_value()) throw std::runtime_error("File already exists");
  files_.push_back(file);

  files_[0].set_size(FileData::FileSize{FileData::file_data_size() * files_.size()});
  if (files_[0].first_cluster_index() == files_[1].first_cluster_index()) {
    files_[1].set_size(FileData::FileSize{FileData::file_data_size() * files_.size()});
  }
}

auto Directory::make_root() -> Directory {
  auto parent_dir = FileData("..", FileData::FileSize{0}, 0, true);
  return make_empty(parent_dir, 0);
}

auto Directory::make_empty(const FileData &parent_dir, std::uint64_t cluster_index) -> Directory {
  if (!parent_dir.is_directory()) throw std::runtime_error("Cannot create directory in file");

  auto current_dir = FileData(".", FileData::FileSize{FileData::file_data_size() * 2}, cluster_index, true);
  auto parent_dir_ref = FileData("..", FileData::FileSize{0}, parent_dir.first_cluster_index(), true);

  auto files = std::vector<FileData>{current_dir, parent_dir_ref};

  return Directory(files);
}
