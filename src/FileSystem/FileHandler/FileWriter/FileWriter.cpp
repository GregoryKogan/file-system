#include "FileWriter.hpp"

#include <utility>

FileWriter::FileWriter(ByteWriter byte_writer, MetadataHandler metadata_handler, std::uint64_t offset)
    : FileHandler(std::move(metadata_handler), offset), byte_writer_(std::move(byte_writer)) {}

auto FileWriter::write(const std::vector<std::byte> &bytes) -> void {
  auto meta = get_metadata_handler().read_metadata();

  auto new_size = byte_writer_.write_bytes(Metadata::get_metadata_size() + get_offset() + get_handled_size(), bytes);

  if (new_size > meta.get_size()) {
    meta.set_size(new_size);
    get_metadata_handler().write_metadata(meta);
  }
}

auto FileWriter::write_next(const std::vector<std::byte> &bytes) -> void {
  write(bytes);
  increase_handled_size(bytes.size());
}
