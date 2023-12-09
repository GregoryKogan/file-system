#include "DiskWriter.hpp"

#include <utility>

DiskWriter::DiskWriter(std::shared_ptr<std::ostream> stream, std::uint64_t offset)
    : DiskHandler(offset), os_(std::move(stream)) {}

void DiskWriter::write(const std::vector<std::byte> &bytes) const {
  os_->seekp(static_cast<std::streamoff>(get_offset() + get_handled_size()));
  os_->write(reinterpret_cast<const char *>(bytes.data()), // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
             static_cast<std::streamsize>(bytes.size()));
  os_->flush();
}

void DiskWriter::write_next_block(const std::vector<std::byte> &bytes) {
  write(bytes);
  increase_handled_size(bytes.size());
}
