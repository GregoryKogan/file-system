#pragma once

#include "../../Metadata/Metadata.hpp"
#include "../ByteReader/ByteReader.hpp"
#include "../ByteWriter/ByteWriter.hpp"
#include <utility>

class MetadataHandler {
  ByteReader byte_reader_;
  ByteWriter byte_writer_;

public:
  MetadataHandler(ByteReader byte_reader, ByteWriter byte_writer);

  [[nodiscard]] auto read_metadata() -> Metadata;
  auto write_metadata(const Metadata &metadata) -> void;
};