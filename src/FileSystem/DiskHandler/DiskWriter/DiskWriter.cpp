#include "DiskWriter.hpp"

DiskWriter::DiskWriter() : DiskHandler(0), os_(nullptr) {}

DiskWriter::DiskWriter(std::shared_ptr<std::ostream> stream, std::uint64_t offset)
    : DiskHandler(offset), os_(std::move(stream)) {}

auto DiskWriter::write(const std::vector<std::byte> &bytes) const -> void {
  os_->seekp(static_cast<std::streamoff>(get_offset() + get_handled_size()));
  os_->write(reinterpret_cast<const char *>(bytes.data()), // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
             static_cast<std::streamsize>(bytes.size()));
  os_->flush();
}

auto DiskWriter::write_next(const std::vector<std::byte> &bytes) -> void {
  write(bytes);
  increase_handled_size(bytes.size());
}
