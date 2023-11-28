#include "FileSystem.hpp"

FileSystem::FileSystem(std::string const &path) {
  auto ifs = std::make_unique<std::ifstream>(path, std::ios::binary);
  if (!ifs->is_open()) throw std::runtime_error("Cannot open file " + path);

  fs_reader_ = std::make_shared<FileReader>(std::move(ifs), FileHandler::FileOffset(0));

  read_settings();

  fat_ = std::make_unique<FAT>(fs_reader_, settings_);
}

auto FileSystem::get_settings() const noexcept -> Settings const & { return settings_; }

void FileSystem::read_settings() {
  fs_reader_->set_offset(FileHandler::FileOffset(0));
  fs_reader_->set_block_size(FileReader::BlockSize(sizeof(std::uint64_t)));

  auto size_bytes = fs_reader_->read_next_block();
  settings_.size = Converter::to_uint64(size_bytes);

  auto cluster_size_bytes = fs_reader_->read_next_block();
  settings_.cluster_size = Converter::to_uint64(cluster_size_bytes);
}

auto operator<<(std::ostream &out_stream, FileSystem const &file_system) -> std::ostream & {
  out_stream << "FileSystem:\n";
  out_stream << "Settings:\n";
  out_stream << "    Size: " << file_system.settings_.size << '\n';
  out_stream << "    Cluster size: " << file_system.settings_.cluster_size << '\n';
  out_stream << FAT::pretty_print_fat(*file_system.fat_);
  return out_stream;
}
