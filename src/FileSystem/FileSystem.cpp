#include "FileSystem.hpp"

void FileSystem::make_fs(std::string const &path, Settings const &settings, bool allow_big) {
  validate_settings(settings, allow_big);

  auto ofs = std::make_unique<std::ofstream>(path, std::ios::binary);
  FileWriter writer(std::move(ofs), FileHandler::FileOffset(0));

  fill_zeros(writer, settings.size);
  write_settings(writer, settings);
  write_fat(writer, settings);
}

void FileSystem::validate_settings(Settings const &settings, bool allow_big) {
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

void FileSystem::fill_zeros(FileWriter &writer, std::uint64_t size) {
  writer.set_offset(FileHandler::FileOffset(0));

  // write full blocks
  std::vector<std::byte> bytes(MAX_BLOCK_SIZE, std::byte{0});
  while (writer.handled_size() + MAX_BLOCK_SIZE < size) writer.write_next_block(bytes);

  // write remaining bytes
  bytes.resize(size - writer.handled_size());
  writer.write_next_block(bytes);
}

void FileSystem::write_settings(FileWriter &writer, Settings const &settings) {
  writer.set_offset(FileHandler::FileOffset(0));

  auto size_bytes = to_bytes(settings.size);
  writer.write_next_block(size_bytes);

  auto cluster_size_bytes = to_bytes(settings.cluster_size);
  writer.write_next_block(cluster_size_bytes);
}

auto FileSystem::to_bytes(std::uint64_t value) -> std::vector<std::byte> {
  const uint8_t BYTES_IN_UINT64 = 8;
  const uint8_t BITS_IN_BYTE = 8;

  std::vector<std::byte> bytes;
  bytes.resize(BYTES_IN_UINT64);

  for (std::size_t i = 0; i < BYTES_IN_UINT64; ++i) {
    bytes[BYTES_IN_UINT64 - i - 1] = static_cast<std::byte>(value >> BITS_IN_BYTE * i);
  }

  return bytes;
}

void FileSystem::write_fat(FileWriter &writer, Settings const &settings) {
  writer.set_offset(FileHandler::FileOffset(SETTINGS_SIZE));

  auto entries_count = calculate_fat_entries_count(settings);
  for (std::uint64_t i = 0; i < entries_count; ++i) {
    std::vector<std::byte> bytes(FAT_ENTRY_SIZE, std::byte{0});
    bytes[0] = ClusterStatus::FREE;
    writer.write_next_block(bytes);
  }
}

auto FileSystem::calculate_fat_entries_count(Settings const &settings) -> std::uint64_t {
  return (settings.size - SETTINGS_SIZE) / (FAT_ENTRY_SIZE + settings.cluster_size);
}
