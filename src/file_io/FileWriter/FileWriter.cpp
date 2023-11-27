#include "FileWriter.hpp"

FileWriter::FileWriter(std::unique_ptr<std::ostream> stream, FileOffset offset)
    : FileHandler(offset, BlockSize{1}), os_(std::move(stream)) {}

void FileWriter::write(const std::vector<std::byte> &bytes) const {
  os_->seekp(offset() + handled_size());
  for (auto byte : bytes) *os_ << static_cast<char>(byte);
  os_->flush();
}

void FileWriter::write_next_block(const std::vector<std::byte> &bytes) {
  write(bytes);
  increase_handled_size(bytes.size());
}
