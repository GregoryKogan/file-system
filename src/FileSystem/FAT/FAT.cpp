#include "FAT.hpp"

FAT::FAT(std::shared_ptr<DiskReader> &disk_reader_, std::shared_ptr<DiskWriter> &disk_writer_,
         FSMaker::Settings const &settings)
    : disk_reader_(disk_reader_), disk_writer_(disk_writer_),
      entries_count_(FSMaker::calculate_fat_entries_count(settings)) {}

auto FAT::entries_count() const noexcept -> std::uint64_t { return entries_count_; }

auto FAT::entries() const -> std::vector<FATEntry> {
  if (entries_count_ > MAX_ENTRIES_TO_LOAD) throw std::runtime_error("Too many FAT entries to load");

  disk_reader_->set_offset(DiskHandler::DiskOffset(FSMaker::FAT_OFFSET));
  disk_reader_->set_block_size(DiskReader::BlockSize(FSMaker::FAT_ENTRY_SIZE));

  std::vector<FATEntry> entries;
  for (std::uint64_t i = 0; i < entries_count_; ++i) {
    auto entry_bytes = disk_reader_->read_next_block();
    auto entry = to_fat_entry(entry_bytes);
    entries.push_back(entry);
  }

  return entries;
}

auto FAT::to_fat_entry(std::vector<std::byte> const &entry_bytes) -> FATEntry {
  if (entry_bytes.size() != FSMaker::FAT_ENTRY_SIZE) throw std::runtime_error("Invalid FAT entry size");

  auto status = entry_bytes[0];
  if (status != FSMaker::ClusterStatusOptions::FREE && status != FSMaker::ClusterStatusOptions::BUSY &&
      status != FSMaker::ClusterStatusOptions::LAST) {
    throw std::runtime_error("Invalid FAT entry status");
  }

  auto next_cluster_bytes = std::vector<std::byte>(entry_bytes.begin() + 1, entry_bytes.end());
  auto next_cluster = Converter::to_uint64(next_cluster_bytes);

  return FATEntry{status, next_cluster};
}

auto FAT::pretty_print_fat(FAT const &fat) -> std::string {
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
  case FSMaker::ClusterStatusOptions::FREE: return "FREE";
  case FSMaker::ClusterStatusOptions::BUSY: return "BUSY";
  case FSMaker::ClusterStatusOptions::LAST: return "LAST";
  default: throw std::runtime_error("Invalid FAT entry status");
  }
}