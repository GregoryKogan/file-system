#include "FileSystem.hpp"

FileSystem::FileSystem(std::string const &path) {
  auto ifs = std::make_shared<std::ifstream>(path, std::ios::binary | std::ios::in);
  auto ofs = std::make_shared<std::ofstream>(path, std::ios::binary | std::ios::out | std::ios::in);
  if (!ifs->is_open() || !ofs->is_open()) throw std::runtime_error("Cannot open file " + path);

  disk_reader_ = DiskReader(std::move(ifs), 0, 0);
  disk_writer_ = DiskWriter(std::move(ofs), 0);

  read_settings();

  fat_ = FAT(disk_reader_, disk_writer_, FSMaker::get_fat_offset(), FSMaker::calculate_fat_entries_count(settings_));

  handler_builder_ = HandlerBuilder(disk_reader_, disk_writer_, fat_,
                                    FSMaker::calculate_clusters_start_offset(settings_), settings_.cluster_size);

  const std::string PATH_DELIMITER = "/";
  path_resolver_ = PathResolver(PATH_DELIMITER, handler_builder_);

  if (!is_root_dir_created()) create_root_dir();
  working_dir_cluster_ = 0;
}

void FileSystem::make(std::string const &path, FSMaker::Settings const &settings, bool allow_big) {
  FSMaker::make_fs(path, settings, allow_big);
}

auto FileSystem::get_settings() const noexcept -> FSMaker::Settings const & { return settings_; }

auto FileSystem::ls(std::string const &path) const -> std::vector<Metadata> {
  auto dir_cluster = path_resolver_.search(path, working_dir_cluster_);
  if (!dir_cluster.has_value()) throw std::invalid_argument("Directory does not exist");

  auto metadata_handler = handler_builder_.build_metadata_handler(dir_cluster.value());
  auto dir_reader = handler_builder_.build_file_reader(dir_cluster.value());
  dir_reader.set_block_size(metadata_handler.read_metadata().get_size());
  auto dir = Directory::from_bytes(dir_reader.read());

  auto child_clusters = dir.list_files();
  std::vector<Metadata> metadata_list;
  for (auto const &child_cluster : child_clusters) {
    auto metadata_handler = handler_builder_.build_metadata_handler(child_cluster);
    metadata_list.push_back(metadata_handler.read_metadata());
  }

  return metadata_list;
}

auto FileSystem::dirname(std::string const &path) -> std::string {
  return PathResolver::dirname(path, path_resolver_.delimiter());
}

auto FileSystem::basename(std::string const &path) -> std::string {
  return PathResolver::basename(path, path_resolver_.delimiter());
}

auto FileSystem::mkdir(std::string const &path) -> void {
  auto parent_dir_path = dirname(path);
  auto dir_name = basename(path);

  auto existing_children = ls(parent_dir_path);
  for (auto const &child : existing_children) {
    if (child.get_name() == dir_name) throw std::invalid_argument("Directory already exists");
  }

  auto parent_dir_cluster = path_resolver_.search(parent_dir_path, working_dir_cluster_);
  if (!parent_dir_cluster.has_value()) throw std::invalid_argument("Parent directory does not exist");

  auto parent_dir_metadata_handler = handler_builder_.build_metadata_handler(parent_dir_cluster.value());
  auto parent_dir_reader = handler_builder_.build_file_reader(parent_dir_cluster.value());
  parent_dir_reader.set_block_size(parent_dir_metadata_handler.read_metadata().get_size());
  auto parent_dir = Directory::from_bytes(parent_dir_reader.read());

  auto new_dir_cluster = fat_.allocate();
  auto new_dir_byte_writer = handler_builder_.build_byte_writer(new_dir_cluster);
  auto new_dir_meta = Metadata(dir_name, 0, new_dir_cluster, parent_dir_cluster.value(), true);
  new_dir_byte_writer.write_bytes(0, new_dir_meta.to_bytes());

  parent_dir.add_file(new_dir_cluster);

  auto parent_dir_writer = handler_builder_.build_file_writer(parent_dir_cluster.value());
  parent_dir_writer.write(parent_dir.to_bytes());
}

void FileSystem::read_settings() {
  disk_reader_.set_offset(0);
  disk_reader_.set_block_size(Converter::get_uint64_size());

  auto size_bytes = disk_reader_.read_next();
  settings_.size = Converter::to_uint64(size_bytes);

  auto cluster_size_bytes = disk_reader_.read_next();
  settings_.cluster_size = Converter::to_uint64(cluster_size_bytes);
}

auto FileSystem::is_root_dir_created() noexcept -> bool { return fat_.is_allocated(0); }

auto FileSystem::create_root_dir() -> void {
  auto first_cluster = fat_.allocate();
  auto byte_writer = handler_builder_.build_byte_writer(first_cluster);
  auto root_meta = Metadata("root", 0, first_cluster, first_cluster, true);
  byte_writer.write_bytes(0, root_meta.to_bytes());
}

// static auto FileSystem::root_dir_size() noexcept -> std::uint64_t {
//   auto file_reader =
//       FileReader(FileData(path_resolver_->delimiter(), FileData::FileSize{FileData::file_data_size()}, 0, true),
//                  FileHandler::FileOffset(0), fat_, settings_.cluster_size, disk_reader_,
//                  FileReader::BlockSize(FileData::file_data_size()));
//   auto root_dir_file_data = FileData::from_bytes(file_reader.read());
//   return root_dir_file_data.size().bytes;
// }

// auto FileSystem::root_dir_file_data() const -> FileData {
//   return FileData(path_resolver_->delimiter(), FileData::FileSize{root_dir_size()}, 0, true);
// }

// auto FileSystem::search(std::string const &path) const -> std::optional<FileData> {
//   return path_resolver_->search(path, *working_dir_, root_dir_file_data());
// }

// static auto FileSystem::get_parent_dir_data(std::string const &path) -> std::optional<FileData> {
//   auto parent_dir_path = dirname(path);
//   return search(parent_dir_path);
// }

// auto FileSystem::write_new_dir(FileData const &parent_dir_data, std::string const &dir_name) -> FileData {
//   auto first_cluster = fat_->allocate();
//   FileData new_dir_data(dir_name, FileData::FileSize{0}, first_cluster, true);
//   auto file_writer = FileWriter(new_dir_data, FileHandler::FileOffset(0), fat_, settings_.cluster_size,
//   disk_writer_); auto dir_bytes = Directory::make_empty(parent_dir_data, first_cluster).to_bytes();
//   file_writer.write_block(dir_bytes);

//   new_dir_data.set_size(FileData::FileSize{dir_bytes.size()});

//   return new_dir_data;
// }

// static auto FileSystem::update_parent_dir(FileData const &parent_dir_data, FileData const &new_file_data) -> void {
//   auto parent_dir_reader = FileReader(parent_dir_data, FileHandler::FileOffset(0), fat_, settings_.cluster_size,
//                                       disk_reader_, FileReader::BlockSize(parent_dir_data.size().bytes));
//   auto parent_dir = Directory::from_bytes(parent_dir_reader.read());
//   parent_dir.add_file(new_file_data);

//   auto parent_dir_writer =
//       FileWriter(parent_dir_data, FileHandler::FileOffset(0), fat_, settings_.cluster_size, disk_writer_);
//   parent_dir_writer.write_block(parent_dir.to_bytes());

//   if (parent_dir_data.first_cluster_index() == working_dir_->first_cluster_index()) {
//     working_dir_->set_size(FileData::FileSize{parent_dir.to_bytes().size()});
//   }
// }

auto operator<<(std::ostream &out_stream, FileSystem const &file_system) -> std::ostream & {
  out_stream << "FileSystem:\n";
  out_stream << "Settings:\n";
  out_stream << "    Size: " << file_system.settings_.size << '\n';
  out_stream << "    Cluster size: " << file_system.settings_.cluster_size << '\n';
  try {
    out_stream << FAT::to_string(file_system.fat_);
  } catch (std::exception const &e) { out_stream << "FAT: " << e.what() << '\n'; }
  return out_stream;
}
