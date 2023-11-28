#include "FAT.hpp"

FAT::FAT(std::shared_ptr<FileReader> &fs_reader, FSMaker::Settings const &settings)
    : fs_reader_(fs_reader), entries_count_(FSMaker::calculate_fat_entries_count(settings)) {}

auto FAT::entries_count() const noexcept -> std::uint64_t { return entries_count_; }

auto FAT::entries() const -> std::vector<FATEntry> {
  fs_reader_->set_offset(FileHandler::FileOffset(FSMaker::FAT_OFFSET));
  fs_reader_->set_block_size(FileReader::BlockSize(FSMaker::FAT_ENTRY_SIZE));

  std::vector<FATEntry> entries;
  for (std::uint64_t i = 0; i < entries_count_; ++i) {
    auto entry_bytes = fs_reader_->read_next_block();
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
  std::ostringstream oss;

  oss << "FAT entries count: " << fat.entries_count() << '\n';
  oss << "FAT entries:" << '\n';

  auto entries = fat.entries();
  for (std::size_t i = 0; i < entries.size(); ++i) {
    auto entry = entries[i];
    oss << "  " << i << ": " << to_string(entries[i].status) << " " << entry.next_cluster << '\n';
  }

  return oss.str();
}

auto FAT::to_string(std::byte status) -> std::string {
  switch (status) {
  case FSMaker::ClusterStatusOptions::FREE: return "FREE";
  case FSMaker::ClusterStatusOptions::BUSY: return "BUSY";
  case FSMaker::ClusterStatusOptions::LAST: return "LAST";
  default: throw std::runtime_error("Invalid FAT entry status");
  }
}