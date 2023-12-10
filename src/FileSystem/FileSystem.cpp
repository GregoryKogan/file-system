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

auto FileSystem::make(std::string const &path, FSMaker::Settings const &settings, bool allow_big) -> void {
  FSMaker::make_fs(path, settings, allow_big);
}

auto FileSystem::get_settings() const noexcept -> FSMaker::Settings const & { return settings_; }

auto FileSystem::ls(std::string const &path) const -> std::vector<Metadata> {
  auto dir_cluster = search(path);
  if (!dir_cluster.has_value()) throw std::invalid_argument("Directory does not exist");

  auto dir = read_dir(dir_cluster.value());
  auto child_clusters = dir.list_files();
  return get_metadata_from_clusters(child_clusters);
}

auto FileSystem::dirname(std::string const &path) const -> std::string {
  return PathResolver::dirname(path, path_resolver_.delimiter());
}

auto FileSystem::basename(std::string const &path) const -> std::string {
  return PathResolver::basename(path, path_resolver_.delimiter());
}

auto FileSystem::mkdir(std::string const &path) -> void {
  if (does_file_exist(path)) throw std::invalid_argument("Already exists");
  if (!does_dir_exist(dirname(path))) throw std::invalid_argument("Parent directory does not exist");

  auto parent_dir_cluster = search(dirname(path));
  if (!parent_dir_cluster.has_value()) throw std::invalid_argument("Parent directory does not exist");

  auto new_dir_cluster = alloc_new_dir(basename(path), parent_dir_cluster.value());
  add_file_to_dir(parent_dir_cluster.value(), new_dir_cluster);
}

auto FileSystem::read_settings() -> void {
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

auto FileSystem::read_dir(std::uint64_t cluster) const -> Directory {
  auto metadata_handler = handler_builder_.build_metadata_handler(cluster);
  auto dir_reader = handler_builder_.build_file_reader(cluster);
  dir_reader.set_block_size(metadata_handler.read_metadata().get_size());
  return Directory::from_bytes(dir_reader.read());
}

auto FileSystem::get_metadata_from_clusters(const std::vector<std::uint64_t> &clusters) const -> std::vector<Metadata> {
  std::vector<Metadata> metadata_list;
  metadata_list.reserve(clusters.size());
  std::transform(clusters.begin(), clusters.end(), std::back_inserter(metadata_list), [this](auto const &cluster) {
    return handler_builder_.build_metadata_handler(cluster).read_metadata();
  });
  return metadata_list;
}

auto FileSystem::search(std::string const &path) const -> std::optional<std::uint64_t> {
  return path_resolver_.search(path, working_dir_cluster_);
}

auto FileSystem::does_file_exist(std::string const &path) const -> bool { return search(path).has_value(); }

auto FileSystem::does_dir_exist(std::string const &path) const -> bool {
  auto dir_cluster = search(path);
  if (!dir_cluster.has_value()) return false;

  auto metadata_handler = handler_builder_.build_metadata_handler(dir_cluster.value());
  return metadata_handler.read_metadata().is_directory();
}

auto FileSystem::alloc_new_dir(std::string const &name, std::uint64_t parent_cluster) -> std::uint64_t {
  auto new_dir_cluster = fat_.allocate();
  auto new_dir_byte_writer = handler_builder_.build_byte_writer(new_dir_cluster);
  auto new_dir_meta = Metadata(name, 0, new_dir_cluster, parent_cluster, true);
  new_dir_byte_writer.write_bytes(0, new_dir_meta.to_bytes());
  return new_dir_cluster;
}

auto FileSystem::add_file_to_dir(std::uint64_t parent_cluster, std::uint64_t child_cluster) const -> void {
  auto parent_dir = read_dir(parent_cluster);
  parent_dir.add_file(child_cluster);
  handler_builder_.build_file_writer(parent_cluster).write(parent_dir.to_bytes());
}

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
