#include "MetadataHandler.hpp"

MetadataHandler::MetadataHandler(ByteReader byte_reader, ByteWriter byte_writer)
    : byte_reader_(std::move(std::move(byte_reader))), byte_writer_(std::move(std::move(byte_writer))) {}

auto MetadataHandler::read_metadata() -> Metadata {
  auto metadata_bytes = byte_reader_.read_bytes(0, Metadata::get_metadata_size());
  return Metadata::from_bytes(metadata_bytes);
}

auto MetadataHandler::write_metadata(const Metadata &metadata) -> void {
  auto metadata_bytes = metadata.to_bytes();
  byte_writer_.write_bytes(0, metadata_bytes);
}
