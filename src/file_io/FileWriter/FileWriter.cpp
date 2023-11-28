#include "FileWriter.hpp"

FileWriter::FileWriter(std::unique_ptr<std::ostream> stream, FileOffset offset)
    : FileHandler(offset, BlockSize{1}), os_(std::move(stream)) {}

void FileWriter::write(const std::vector<std::byte> &bytes) const {
  os_->seekp(offset() + handled_size());
  os_->write(reinterpret_cast<const char *>(bytes.data()), // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
             static_cast<std::streamsize>(bytes.size()));
  os_->flush();
}

void FileWriter::write_next_block(const std::vector<std::byte> &bytes) {
  write(bytes);
  increase_handled_size(bytes.size());
}
