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

  const std::string PATH_DELIMITER = "/";
  path_resolver_ = std::make_unique<PathResolver>(PATH_DELIMITER, disk_reader_, fat_, settings_.cluster_size);

  if (!is_root_dir_created()) create_root_dir();
  working_dir_ = std::make_shared<FileData>("/", FileData::FileSize{root_dir_size()}, 0, true);
}

void FileSystem::make(std::string const &path, FSMaker::Settings const &settings, bool allow_big) {
  FSMaker::make_fs(path, settings, allow_big);
}

auto FileSystem::get_settings() const noexcept -> FSMaker::Settings const & { return settings_; }

auto FileSystem::ls(std::string const &path) const -> std::vector<FileData> {
  auto size = root_dir_size();
  auto file_reader = FileReader(FileData("/", FileData::FileSize{size}, 0, true), FileHandler::FileOffset(0), fat_,
                                settings_.cluster_size, disk_reader_, FileReader::BlockSize(size));
  auto root_dir = Directory::from_bytes(file_reader.read_block());
  return root_dir.files();
}

auto FileSystem::dirname(std::string const &path) -> std::string {
  return PathResolver::dirname(path, path_resolver_->delimiter());
}

auto FileSystem::basename(std::string const &path) -> std::string {
  return PathResolver::basename(path, path_resolver_->delimiter());
}

void FileSystem::read_settings() {
  disk_reader_->set_offset(DiskHandler::DiskOffset(0));
  disk_reader_->set_block_size(DiskReader::BlockSize(sizeof(std::uint64_t)));

  auto size_bytes = disk_reader_->read_next_block();
  settings_.size = Converter::to_uint64(size_bytes);

  auto cluster_size_bytes = disk_reader_->read_next_block();
  settings_.cluster_size = Converter::to_uint64(cluster_size_bytes);
}

auto FileSystem::is_root_dir_created() const noexcept -> bool { return fat_->is_allocated(0); }

auto FileSystem::create_root_dir() -> void {
  auto first_cluster = fat_->allocate();
  auto file_writer = FileWriter(FileData("/", FileData::FileSize{0}, first_cluster, true), FileHandler::FileOffset(0),
                                fat_, settings_.cluster_size, disk_writer_);

  auto root_dir_bytes = Directory::make_root().to_bytes();
  file_writer.write_block(root_dir_bytes);
}

auto FileSystem::root_dir_size() const noexcept -> std::uint64_t {
  auto file_reader =
      FileReader(FileData("/", FileData::FileSize{FileData::file_data_size()}, 0, true), FileHandler::FileOffset(0),
                 fat_, settings_.cluster_size, disk_reader_, FileReader::BlockSize(FileData::file_data_size()));
  auto root_dir_file_data = FileData::from_bytes(file_reader.read_block());
  return root_dir_file_data.size().bytes;
}

auto FileSystem::root_dir_file_data() const -> FileData {
  return FileData("/", FileData::FileSize{root_dir_size()}, 0, true);
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
