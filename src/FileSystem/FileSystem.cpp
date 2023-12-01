#include "FileSystem.hpp"

FileSystem::FileSystem(std::string const &path) {
  auto ifs = std::make_unique<std::ifstream>(path, std::ios::binary | std::ios::in);
  auto ofs = std::make_unique<std::ofstream>(path, std::ios::binary | std::ios::out | std::ios::in);
  if (!ifs->is_open() || !ofs->is_open()) throw std::runtime_error("Cannot open file " + path);

  disk_reader_ = std::make_shared<DiskReader>(std::move(ifs), DiskHandler::DiskOffset(0));
  disk_writer_ = std::make_shared<DiskWriter>(std::move(ofs), DiskHandler::DiskOffset(0));

  read_settings();

  fat_ = std::make_shared<FAT>(disk_reader_, disk_writer_, DiskHandler::DiskOffset(FSMaker::fat_offset()),
                               FSMaker::calculate_fat_entries_count(settings_));
}

void FileSystem::make(std::string const &path, FSMaker::Settings const &settings, bool allow_big) {
  FSMaker::make_fs(path, settings, allow_big);

  FileSystem file_system(path);
  auto cluster_size = file_system.get_settings().cluster_size;

  auto root_dir_bytes = Directory::make_root().to_bytes();
  std::uint64_t bytes_written = 0;
  auto cur_cluster = file_system.fat_->allocate();
  while (root_dir_bytes.size() - bytes_written >= cluster_size) {
    auto cluster_offset = file_system.fat_->cluster_offset(cur_cluster, cluster_size);
    file_system.disk_writer_->set_offset(cluster_offset);
    std::vector<std::byte> cluster_bytes(root_dir_bytes.begin() + static_cast<std::int64_t>(bytes_written),
                                         root_dir_bytes.begin() +
                                             static_cast<std::int64_t>(bytes_written + cluster_size));
    file_system.disk_writer_->write(cluster_bytes);
    bytes_written += cluster_size;

    if (bytes_written < root_dir_bytes.size()) {
      auto next_cluster = file_system.fat_->allocate();
      file_system.fat_->set_next(cur_cluster, next_cluster);
      cur_cluster = next_cluster;
    }
  }
  auto last_cluster_bytes =
      std::vector<std::byte>(root_dir_bytes.begin() + static_cast<std::int64_t>(bytes_written), root_dir_bytes.end());
  auto last_cluster_offset = file_system.fat_->cluster_offset(cur_cluster, cluster_size);
  file_system.disk_writer_->set_offset(last_cluster_offset);
  file_system.disk_writer_->write(last_cluster_bytes);
}

auto FileSystem::get_settings() const noexcept -> FSMaker::Settings const & { return settings_; }

void FileSystem::read_settings() {
  disk_reader_->set_offset(DiskHandler::DiskOffset(0));
  disk_reader_->set_block_size(DiskReader::BlockSize(sizeof(std::uint64_t)));

  auto size_bytes = disk_reader_->read_next_block();
  settings_.size = Converter::to_uint64(size_bytes);

  auto cluster_size_bytes = disk_reader_->read_next_block();
  settings_.cluster_size = Converter::to_uint64(cluster_size_bytes);
}

auto operator<<(std::ostream &out_stream, FileSystem const &file_system) -> std::ostream & {
  out_stream << "FileSystem:\n";
  out_stream << "Settings:\n";
  out_stream << "    Size: " << file_system.settings_.size << '\n';
  out_stream << "    Cluster size: " << file_system.settings_.cluster_size << '\n';
  try {
    out_stream << FAT::to_string(*file_system.fat_);
  } catch (std::exception const &e) { out_stream << "FAT: " << e.what() << '\n'; }
  return out_stream;
}
