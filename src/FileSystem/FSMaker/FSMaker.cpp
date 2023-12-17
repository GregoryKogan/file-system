#include "FSMaker.hpp"

auto FSMaker::make_fs(std::string const &path, Settings const &settings, bool allow_big) -> void {
  validate_settings(settings, allow_big);

  auto ofs = std::make_unique<std::ofstream>(path, std::ios::binary);
  if (!ofs->is_open()) throw std::runtime_error("Cannot open file " + path);

  DiskWriter writer(std::move(ofs), 0);

  fill_zeros(writer, settings.size);
  write_settings(writer, settings);
  write_fat(writer, settings);
}

auto FSMaker::get_fat_offset() -> std::uint64_t { return FAT_OFFSET; }

auto FSMaker::validate_settings(Settings const &settings, bool allow_big) -> void {
  if (settings.size < MIN_FS_SIZE) {
    throw std::runtime_error("File system size must be greater than " + std::to_string(MIN_FS_SIZE) + " bytes");
  }

  if (settings.cluster_size < MIN_CLUSTER_SIZE) {
    throw std::runtime_error("Cluster size must be greater than " + std::to_string(MIN_CLUSTER_SIZE) + " bytes");
  }

  if (settings.size - settings.cluster_size < MIN_FS_SIZE) {
    throw std::runtime_error("Cluster size is too big for this file system size. "
                             "File system size must be greater than " +
                             std::to_string(MIN_FS_SIZE + settings.cluster_size) + " bytes");
  }

  if (!allow_big && settings.size > BIG_THRESHOLD) {
    throw std::runtime_error("File system size is too big. "
                             "Use allow_big flag to allow file systems bigger than " +
                             std::to_string(BIG_THRESHOLD) + " bytes");
  }
}

auto FSMaker::fill_zeros(DiskWriter &writer, std::uint64_t size) -> void {
  writer.set_offset(0);

  // write full blocks
  std::vector<std::byte> bytes(MAX_BLOCK_SIZE, std::byte{0});
  while (writer.get_handled_size() + MAX_BLOCK_SIZE < size) writer.write_next(bytes);

  // write remaining bytes
  bytes.resize(size - writer.get_handled_size());
  writer.write_next(bytes);
}

auto FSMaker::write_settings(DiskWriter &writer, Settings const &settings) -> void {
  writer.set_offset(0);

  auto size_bytes = Converter::to_bytes(settings.size);
  writer.write_next(size_bytes);

  auto cluster_size_bytes = Converter::to_bytes(settings.cluster_size);
  writer.write_next(cluster_size_bytes);
}

auto FSMaker::write_fat(DiskWriter &writer, Settings const &settings) -> void {
  writer.set_offset(FAT_OFFSET);

  auto entries_count = calculate_fat_entries_count(settings);
  for (std::uint64_t i = 0; i < entries_count; ++i) { writer.write_next(FAT::get_empty_entry_bytes()); }
}

auto FSMaker::calculate_fat_entries_count(Settings const &settings) -> std::uint64_t {
  return (settings.size - SETTINGS_SIZE) / (FAT::get_entry_size() + settings.cluster_size);
}

auto FSMaker::calculate_clusters_start_offset(Settings const &settings) -> std::uint64_t {
  return get_fat_offset() + calculate_fat_entries_count(settings) * FAT::get_entry_size();
}
