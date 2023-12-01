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

auto Directory::make_root() -> Directory {
  auto current_dir = FileData(".", FileData::FileSize{FileData::file_data_size() * 2}, 0, true);
  auto parent_dir = FileData("..", FileData::FileSize{FileData::file_data_size() * 2}, 0, true);

  auto files = std::vector<FileData>{current_dir, parent_dir};

  return Directory(files);
}
