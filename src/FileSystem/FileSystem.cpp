#include "FileSystem.hpp"

void FileSystem::make_fs(std::string const &path, Settings const &settings) {
  auto ofs = std::make_unique<std::ofstream>(path, std::ios::binary);
  FileWriter writer(std::move(ofs), FileHandler::FileOffset(0));

  fill_zeros(writer, settings.size);
  write_settings(writer, settings);
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
