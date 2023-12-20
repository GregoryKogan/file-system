#include "FileSystem.hpp"

FileSystem::FileSystem(std::string const &path) {
  auto ifs = std::make_shared<std::ifstream>(path, std::ios::binary | std::ios::in);
  auto ofs = std::make_shared<std::ofstream>(path, std::ios::binary | std::ios::out | std::ios::in);
  if (!ifs->is_open() || !ofs->is_open()) throw std::runtime_error("Cannot open file " + path);

  disk_reader_ = DiskReader(std::move(ifs), 0, 0);
  disk_writer_ = DiskWriter(std::move(ofs), 0);

  if (!check_signature()) throw std::runtime_error("Specified file is not a file system");
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

auto FileSystem::pwd() const -> std::string { return path_resolver_.trace(working_dir_cluster_); }

auto FileSystem::ls(std::string const &path) const -> std::vector<Metadata> {
  auto dir_cluster = search(path);
  if (!dir_cluster.has_value()) throw std::invalid_argument("Directory does not exist");

  auto dir = read_dir(dir_cluster.value());
  auto child_clusters = dir.list_files();
  return get_metadata_from_clusters(child_clusters);
}

auto FileSystem::stat(std::string const &path) const -> Metadata {
  auto file_cluster = search(path);
  if (!file_cluster.has_value()) throw std::invalid_argument("File does not exist");
  return handler_builder_.build_metadata_handler(file_cluster.value()).read_metadata();
}

auto FileSystem::dirname(std::string const &path) const -> std::string {
  return PathResolver::dirname(path, path_resolver_.delimiter());
}

auto FileSystem::basename(std::string const &path) const -> std::string {
  return PathResolver::basename(path, path_resolver_.delimiter());
}

auto FileSystem::get_reader(std::string const &path) const -> FileReader {
  auto file_cluster = search(path);
  if (!file_cluster.has_value()) throw std::invalid_argument("File does not exist");
  if (handler_builder_.build_metadata_handler(file_cluster.value()).read_metadata().is_directory()) {
    throw std::invalid_argument("Cannot open directory with get_reader");
  }

  auto file_reader = handler_builder_.build_file_reader(file_cluster.value());
  file_reader.set_block_size(settings_.cluster_size);
  file_reader.set_offset(0);
  return file_reader;
}

auto FileSystem::get_writer(std::string const &path) -> FileWriter {
  auto file_cluster = search(path);
  if (!file_cluster.has_value()) throw std::invalid_argument("File does not exist");
  if (handler_builder_.build_metadata_handler(file_cluster.value()).read_metadata().is_directory()) {
    throw std::invalid_argument("Cannot open directory with get_writer");
  }

  auto file_writer = handler_builder_.build_file_writer(file_cluster.value());
  file_writer.set_offset(0);
  return file_writer;
}

auto FileSystem::mkdir(std::string const &path) -> void {
  if (does_exist(path)) throw std::invalid_argument("Already exists");
  auto parent_dir_cluster = search(dirname(path));
  if (!parent_dir_cluster.has_value()) throw std::invalid_argument("Parent directory does not exist");

  auto new_dir_cluster = alloc_new_dir(basename(path), parent_dir_cluster.value());
  add_file_to_dir(parent_dir_cluster.value(), new_dir_cluster);
}

auto FileSystem::touch(std::string const &path) -> void {
  if (does_file_exist(path)) return;
  auto parent_dir_cluster = search(dirname(path));
  if (!does_dir_exist(dirname(path)) || !parent_dir_cluster.has_value()) {
    throw std::invalid_argument("Parent directory does not exist");
  }

  auto new_file_cluster = fat_.allocate();
  auto new_file_byte_writer = handler_builder_.build_byte_writer(new_file_cluster);
  auto new_file_meta = Metadata(basename(path), 0, new_file_cluster, parent_dir_cluster.value(), false);
  new_file_byte_writer.write_bytes(0, new_file_meta.to_bytes());

  add_file_to_dir(parent_dir_cluster.value(), new_file_cluster);
}

auto FileSystem::rmdir(std::string const &path) -> void {
  auto dir_cluster = search(path);
  if (!does_dir_exist(path) || !dir_cluster.has_value()) throw std::invalid_argument("Directory does not exist");
  if (dir_cluster.value() == 0) throw std::invalid_argument("Cannot remove root directory");
  if (dir_cluster.value() == working_dir_cluster_) throw std::invalid_argument("Cannot remove working directory");
  if (!read_dir(dir_cluster.value()).list_files().empty()) throw std::invalid_argument("Directory is not empty");

  auto parent_dir_cluster =
      handler_builder_.build_metadata_handler(dir_cluster.value()).read_metadata().get_parent_first_cluster();

  fat_.free(dir_cluster.value());
  remove_file_from_dir(parent_dir_cluster, dir_cluster.value());
}

auto FileSystem::rm(std::string const &path, bool recursive) -> void {
  auto file_cluster = search(path);
  if (!file_cluster.has_value()) throw std::invalid_argument("No such file or directory");

  if (recursive) {
    rm_recursive(path);
    return;
  }

  if (handler_builder_.build_metadata_handler(file_cluster.value()).read_metadata().is_directory()) {
    rmdir(path);
    return;
  }

  rmfile(path);
}

auto FileSystem::cp(std::string const &source, std::string const &destination, bool recursive) -> void {
  if (!recursive) {
    shallow_copy(source, destination);
    return;
  }

  deep_copy(source, destination);
}

auto FileSystem::mv(std::string const &source, std::string const &destination, bool recursive) -> void {
  cp(source, destination, recursive);
  rm(source, recursive);
}

auto FileSystem::import_file(std::istream &in_stream, std::string const &path) -> void {
  if (does_dir_exist(path)) throw std::invalid_argument("Cannot import nameless file to directory");
  if (does_file_exist(path)) throw std::invalid_argument("File already exists");

  touch(path);
  auto file_writer = get_writer(path);
  file_writer.set_offset(0);

  in_stream.seekg(0);
  std::istreambuf_iterator<char> iter(in_stream);
  std::istreambuf_iterator<char> end;

  while (iter != end) {
    std::vector<std::byte> buffer;
    buffer.reserve(settings_.cluster_size);
    for (std::size_t i = 0; i < settings_.cluster_size && iter != end; ++iter, ++i) {
      buffer.push_back(static_cast<std::byte>(*iter));
    }
    file_writer.write_next(buffer);
  }
}

auto FileSystem::export_file(std::string const &path, std::ostream &out_stream) const -> void {
  if (!does_file_exist(path)) throw std::invalid_argument("File does not exist");

  auto file_reader = get_reader(path);
  file_reader.set_block_size(settings_.cluster_size);
  file_reader.set_offset(0);

  auto buffer = file_reader.read_next();
  while (!buffer.empty()) {
    out_stream.write(
        reinterpret_cast<char const *>(buffer.data()), // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
        static_cast<std::streamsize>(buffer.size()));
    buffer = file_reader.read_next();
  }

  out_stream.flush();
}

auto FileSystem::rm_recursive(std::string const &path) -> void {
  auto file_cluster = search(path);
  if (!file_cluster.has_value()) throw std::invalid_argument("No such file or directory");

  auto meta = handler_builder_.build_metadata_handler(file_cluster.value()).read_metadata();

  if (!meta.is_directory()) {
    rmfile(path);
    return;
  }

  if (path_resolver_.is_descendant(working_dir_cluster_, meta.get_first_cluster())) {
    throw std::invalid_argument("Cannot remove working directory or its ancestor");
  }

  auto dir = read_dir(meta.get_first_cluster());
  auto child_clusters = dir.list_files();
  for (auto const &child_cluster : child_clusters) { rm_recursive(path_resolver_.trace(child_cluster)); }
  rmdir(path);
}

auto FileSystem::cd(std::string const &path) -> void {
  auto dir_cluster = search(path);
  if (!does_dir_exist(path) || !dir_cluster.has_value()) throw std::invalid_argument("Directory does not exist");
  working_dir_cluster_ = dir_cluster.value();
}

auto FileSystem::cat(std::string const &path, std::ostream &out_stream) const -> void {
  auto file_cluster = search(path);
  if (!file_cluster.has_value()) throw std::invalid_argument("File does not exist");
  if (handler_builder_.build_metadata_handler(file_cluster.value()).read_metadata().is_directory()) {
    throw std::invalid_argument("Cannot cat directory");
  }

  auto file_reader = handler_builder_.build_file_reader(file_cluster.value());
  file_reader.set_block_size(settings_.cluster_size);
  file_reader.set_offset(0);

  auto file_bytes = file_reader.read_next();
  while (!file_bytes.empty()) {
    out_stream << Converter::to_string(file_bytes);
    file_bytes = file_reader.read_next();
  }
}

auto FileSystem::check_signature() -> bool {
  disk_reader_.set_offset(0);
  disk_reader_.set_block_size(FSMaker::get_signature_size());
  auto signature_bytes = disk_reader_.read();
  return Converter::to_string(signature_bytes) == FSMaker::get_signature();
}

auto FileSystem::read_settings() -> void {
  disk_reader_.set_offset(FSMaker::get_settings_offset());
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

auto FileSystem::does_exist(std::string const &path) const -> bool { return search(path).has_value(); }

auto FileSystem::does_file_exist(std::string const &path) const -> bool {
  auto dir_cluster = search(path);
  if (!dir_cluster.has_value()) return false;

  auto metadata_handler = handler_builder_.build_metadata_handler(dir_cluster.value());
  return !metadata_handler.read_metadata().is_directory();
}

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

auto FileSystem::remove_file_from_dir(std::uint64_t parent_cluster, std::uint64_t child_cluster) -> void {
  auto parent_dir = read_dir(parent_cluster);
  parent_dir.remove_file(child_cluster);
  auto parent_meta = handler_builder_.build_metadata_handler(parent_cluster).read_metadata();
  overwrite_file(parent_cluster, parent_meta, parent_dir.to_bytes());
}

auto FileSystem::overwrite_file(std::uint64_t cluster, Metadata old_meta, std::vector<std::byte> const &bytes) -> void {
  auto new_meta = std::move(old_meta);
  new_meta.set_size(bytes.size());
  fat_.shrink(cluster);
  handler_builder_.build_byte_writer(cluster).write_bytes(0, new_meta.to_bytes());
  handler_builder_.build_file_writer(cluster).write(bytes);
}

auto FileSystem::rmfile(std::string const &path) -> void {
  auto file_cluster = search(path);
  if (!file_cluster.has_value()) throw std::invalid_argument("File does not exist");
  if (handler_builder_.build_metadata_handler(file_cluster.value()).read_metadata().is_directory()) {
    throw std::invalid_argument("Cannot remove directory with rmfile");
  }

  auto parent_dir_cluster =
      handler_builder_.build_metadata_handler(file_cluster.value()).read_metadata().get_parent_first_cluster();

  fat_.free(file_cluster.value());
  remove_file_from_dir(parent_dir_cluster, file_cluster.value());
}

auto FileSystem::shallow_copy(std::string const &source, std::string const &destination) -> void {
  if (does_exist(destination)) throw std::invalid_argument("Destination already exists");

  auto source_cluster = search(source);
  if (!source_cluster.has_value()) throw std::invalid_argument("Source does not exist");

  auto source_meta = handler_builder_.build_metadata_handler(source_cluster.value()).read_metadata();
  if (source_meta.is_directory() && !read_dir(source_cluster.value()).list_files().empty()) {
    throw std::invalid_argument("Cannot copy non-empty directory");
  }

  if (source_meta.is_directory()) {
    mkdir(destination);
    return;
  }

  touch(destination);
  auto source_reader = handler_builder_.build_file_reader(source_cluster.value());
  auto destination_cluster = search(destination);
  if (!destination_cluster.has_value()) throw std::invalid_argument("Destination does not exist");
  auto destination_writer = handler_builder_.build_file_writer(destination_cluster.value());

  source_reader.set_block_size(settings_.cluster_size);
  source_reader.set_offset(0);
  destination_writer.set_offset(0);

  auto buffer = source_reader.read_next();
  while (!buffer.empty()) {
    destination_writer.write_next(buffer);
    buffer = source_reader.read_next();
  }
}

auto FileSystem::deep_copy(std::string const &source, std::string const &destination) -> void {
  if (!does_exist(source)) throw std::invalid_argument("Source does not exist");
  if (does_exist(destination)) throw std::invalid_argument("Destination already exists");

  auto source_cluster = search(source);
  if (!source_cluster.has_value()) throw std::invalid_argument("Source does not exist");

  auto source_meta = handler_builder_.build_metadata_handler(source_cluster.value()).read_metadata();
  if (!source_meta.is_directory() || read_dir(source_cluster.value()).list_files().empty()) {
    shallow_copy(source, destination);
    return;
  }

  mkdir(destination);
  auto destination_cluster = search(destination);
  if (!destination_cluster.has_value()) throw std::invalid_argument("Destination does not exist");

  auto source_dir = read_dir(source_cluster.value());
  auto child_clusters = source_dir.list_files();
  for (auto const &child_cluster : child_clusters) {
    auto child_meta = handler_builder_.build_metadata_handler(child_cluster).read_metadata();
    auto child_destination = path_resolver_.trace(destination_cluster.value()) + "/" + child_meta.get_name();
    cp(path_resolver_.trace(child_cluster), child_destination, true);
  }
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
