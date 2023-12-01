#include "FAT.hpp"

FAT::FAT(std::shared_ptr<DiskReader> &disk_reader_, std::shared_ptr<DiskWriter> &disk_writer_,
         DiskHandler::DiskOffset offset, std::uint64_t entries_count)
    : disk_reader_(disk_reader_), disk_writer_(disk_writer_), entries_count_(entries_count), disk_offset_(offset) {}

auto FAT::entries_count() const noexcept -> std::uint64_t { return entries_count_; }

auto FAT::entries() const -> std::vector<FATEntry> {
  if (entries_count_ > MAX_ENTRIES_TO_LOAD) throw std::runtime_error("Too many FAT entries to load");

  disk_reader_->set_offset(disk_offset_);
  disk_reader_->set_block_size(DiskReader::BlockSize(ENTRY_SIZE));

  std::vector<FATEntry> entries;
  for (std::uint64_t i = 0; i < entries_count_; ++i) {
    auto entry_bytes = disk_reader_->read_next_block();
    auto entry = to_fat_entry(entry_bytes);
    entries.push_back(entry);
  }

  return entries;
}

auto FAT::allocate() -> std::uint64_t {
  for (std::uint64_t i = 0; i < entries_count_; ++i) {
    if (!is_allocated(i)) {
      auto entry = FATEntry{ClusterStatusOptions::LAST, 0};
      disk_writer_->set_offset(DiskHandler::DiskOffset(disk_offset_.value + i * ENTRY_SIZE));
      disk_writer_->write(to_bytes(entry));
      return i;
    }
  }

  throw std::runtime_error("Cannot allocate cluster");
}

void FAT::set_next(std::uint64_t cluster_index, std::uint64_t next_cluster_index) {
  if (cluster_index >= entries_count_ || next_cluster_index >= entries_count_) {
    throw std::runtime_error("Invalid cluster index");
  }

  auto entry = get_entry(cluster_index);
  if (entry.status == ClusterStatusOptions::FREE) throw std::runtime_error("Cluster is not allocated");
  if (entry.status == ClusterStatusOptions::ALLOCATED) throw std::runtime_error("Cluster is not last");

  entry.next_cluster = next_cluster_index;
  entry.status = ClusterStatusOptions::ALLOCATED;
  set_entry(cluster_index, entry);
}

auto FAT::get_next(std::uint64_t cluster_index) const -> std::uint64_t {
  auto entry = get_entry(cluster_index);
  if (entry.status == ClusterStatusOptions::FREE) throw std::runtime_error("Cluster is not allocated");
  if (entry.status == ClusterStatusOptions::LAST) throw std::runtime_error("Cluster is last and has no next cluster");
  return entry.next_cluster;
}

auto FAT::is_last(std::uint64_t cluster_index) const -> bool {
  return get_entry(cluster_index).status == ClusterStatusOptions::LAST;
}

auto FAT::cluster_offset(std::uint64_t cluster_index, std::uint64_t cluster_size) const -> DiskHandler::DiskOffset {
  return DiskHandler::DiskOffset(disk_offset_.value + entries_count_ * ENTRY_SIZE + cluster_index * cluster_size);
}

auto FAT::is_allocated(std::uint64_t cluster_index) const -> bool {
  auto entry = get_entry(cluster_index);
  return entry.status != ClusterStatusOptions::FREE;
}

auto FAT::get_entry(std::uint64_t cluster_index) const -> FATEntry {
  if (cluster_index >= entries_count_) throw std::runtime_error("Invalid cluster index");

  std::uint64_t cluster_disk_offset = disk_offset_.value + cluster_index * ENTRY_SIZE;
  disk_reader_->set_offset(DiskHandler::DiskOffset(cluster_disk_offset));
  disk_reader_->set_block_size(DiskReader::BlockSize(ENTRY_SIZE));
  auto entry_bytes = disk_reader_->read_block();
  auto entry = to_fat_entry(entry_bytes);
  return entry;
}

void FAT::set_entry(std::uint64_t cluster_index, FATEntry const &entry) {
  if (cluster_index >= entries_count_) throw std::runtime_error("Invalid cluster index");

  std::uint64_t cluster_disk_offset = disk_offset_.value + cluster_index * ENTRY_SIZE;
  disk_writer_->set_offset(DiskHandler::DiskOffset(cluster_disk_offset));
  disk_writer_->write(to_bytes(entry));
}

auto FAT::to_fat_entry(std::vector<std::byte> const &entry_bytes) -> FATEntry {
  if (entry_bytes.size() != ENTRY_SIZE) throw std::runtime_error("Invalid FAT entry size");

  auto status = entry_bytes[0];
  if (status != ClusterStatusOptions::FREE && status != ClusterStatusOptions::ALLOCATED &&
      status != ClusterStatusOptions::LAST) {
    throw std::runtime_error("Invalid FAT entry status");
  }

  auto next_cluster_bytes = std::vector<std::byte>(entry_bytes.begin() + 1, entry_bytes.end());
  auto next_cluster = Converter::to_uint64(next_cluster_bytes);

  return FATEntry{status, next_cluster};
}

auto FAT::to_bytes(FATEntry const &entry) -> std::vector<std::byte> {
  std::vector<std::byte> bytes(ENTRY_SIZE, std::byte{0});
  bytes[0] = entry.status;

  auto next_cluster_bytes = Converter::to_bytes(entry.next_cluster);
  std::copy(next_cluster_bytes.begin(), next_cluster_bytes.end(), bytes.begin() + 1);

  return bytes;
}

auto FAT::empty_entry_bytes() -> std::vector<std::byte> {
  std::vector<std::byte> bytes(ENTRY_SIZE, std::byte{0});
  bytes[0] = ClusterStatusOptions::FREE;
  return bytes;
}

auto FAT::entry_size() -> std::uint64_t { return ENTRY_SIZE; }

auto FAT::to_string(FAT const &fat) -> std::string {
  auto entries = fat.entries();

  std::ostringstream oss;

  oss << "FAT entries count: " << entries.size() << '\n';
  oss << "FAT entries:" << '\n';

  for (std::size_t i = 0; i < entries.size(); ++i) {
    auto entry = entries[i];
    oss << "  " << i << ": " << cluster_status_to_string(entries[i].status) << " " << entry.next_cluster << '\n';
  }

  return oss.str();
}

auto FAT::cluster_status_to_string(std::byte status) -> std::string {
  switch (status) {
  case ClusterStatusOptions::FREE: return "FREE";
  case ClusterStatusOptions::ALLOCATED: return "ALLOCATED";
  case ClusterStatusOptions::LAST: return "LAST";
  default: throw std::runtime_error("Invalid FAT entry status");
  }
}