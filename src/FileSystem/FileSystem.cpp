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

  auto first_cluster = file_system.fat_->allocate();
  auto file_writer = FileWriter(FileData("/", FileData::FileSize{0}, first_cluster, true), FileHandler::FileOffset(0),
                                file_system.fat_, file_system.settings_.cluster_size, file_system.disk_writer_);

  auto root_dir_bytes = Directory::make_root().to_bytes();
  file_writer.write_next_block(Converter::to_bytes(static_cast<std::uint64_t>(0)));
  auto new_file_size = file_writer.write_next_block(root_dir_bytes);
  file_writer.set_offset(FileHandler::FileOffset(0));
  file_writer.write_block(Converter::to_bytes(new_file_size));
}

auto FileSystem::get_settings() const noexcept -> FSMaker::Settings const & { return settings_; }

auto FileSystem::ls(std::string const &path) const -> std::vector<FileData> {
  const std::uint64_t ROOT_DIR_ENTRIES_OFFSET = 8;
  auto size = root_dir_size();
  auto file_reader =
      FileReader(FileData("/", FileData::FileSize{size}, 0, true), FileHandler::FileOffset(ROOT_DIR_ENTRIES_OFFSET),
                 fat_, settings_.cluster_size, disk_reader_);
  file_reader.set_block_size(FileReader::BlockSize(size));

  auto root_dir_bytes = file_reader.read_block();
  auto root_dir = Directory::from_bytes(root_dir_bytes);

  return root_dir.files();
}

void FileSystem::read_settings() {
  disk_reader_->set_offset(DiskHandler::DiskOffset(0));
  disk_reader_->set_block_size(DiskReader::BlockSize(sizeof(std::uint64_t)));

  auto size_bytes = disk_reader_->read_next_block();
  settings_.size = Converter::to_uint64(size_bytes);

  auto cluster_size_bytes = disk_reader_->read_next_block();
  settings_.cluster_size = Converter::to_uint64(cluster_size_bytes);
}

auto FileSystem::root_dir_size() const noexcept -> std::uint64_t {
  const std::uint64_t UINT64_SIZE = 8;
  auto file_reader = FileReader(FileData("/", FileData::FileSize{UINT64_SIZE}, 0, true), FileHandler::FileOffset(0),
                                fat_, settings_.cluster_size, disk_reader_);
  file_reader.set_block_size(FileReader::BlockSize(UINT64_SIZE));
  return Converter::to_uint64(file_reader.read_block());
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
