#include "FileWriter.hpp"

FileWriter::FileWriter(FileData file_data, FileOffset offset, std::shared_ptr<FAT> fat, std::uint64_t cluster_size,
                       std::shared_ptr<DiskWriter> disk_writer)
    : FileHandler(std::move(file_data), offset, std::move(fat), cluster_size), disk_writer_(std::move(disk_writer)) {}

auto FileWriter::write_block(const std::vector<std::byte> &bytes) -> std::uint64_t {
  auto cur_size = file_data().size().bytes;
  auto out_size = std::max(cur_size, offset() + handled_size() + bytes.size());
  if (out_size > cur_size) { expand_file(out_size); }

  std::uint64_t block_position = 0;
  std::uint64_t file_position = 0;

  auto cur_cluster = file_data().first_cluster_index();
  while (block_position < bytes.size()) {
    auto cluster_offset = fat()->cluster_offset(cur_cluster, cluster_size());
    for (std::uint64_t cluster_inside_offset = 0; cluster_inside_offset < cluster_size(); ++cluster_inside_offset) {
      if (file_position >= offset() + handled_size()) {
        disk_writer_->set_offset(DiskHandler::DiskOffset(cluster_offset.value + cluster_inside_offset));
        disk_writer_->write(std::vector<std::byte>{bytes[block_position]});
        ++block_position;
      }

      ++file_position;
      if (block_position >= bytes.size()) { break; }
    }
    if (block_position < bytes.size()) { cur_cluster = fat()->get_next(cur_cluster); }
  }

  return out_size;
}

auto FileWriter::write_next_block(const std::vector<std::byte> &bytes) -> std::uint64_t {
  auto out_size = write_block(bytes);
  increase_handled_size(bytes.size());
  return out_size;
}

void FileWriter::expand_file(std::uint64_t new_file_size) {
  auto cur_size = file_data().size().bytes;
  if (new_file_size < cur_size) { throw std::runtime_error("New file size is smaller than current"); }

  auto occupied_clusters_count = cur_size / cluster_size() + static_cast<std::uint64_t>(cur_size % cluster_size() != 0);
  if (occupied_clusters_count == 0) occupied_clusters_count = 1;
  auto new_clusters_count =
      new_file_size / cluster_size() + static_cast<std::uint64_t>(new_file_size % cluster_size() != 0);

  auto extra_clusters_count = new_clusters_count - occupied_clusters_count;

  auto cur_cluster = file_data().first_cluster_index();
  while (!fat()->is_last(cur_cluster)) { cur_cluster = fat()->get_next(cur_cluster); }

  auto last_cluster = cur_cluster;
  auto first_extra_cluster = fat()->allocate(extra_clusters_count);
  fat()->set_next(last_cluster, first_extra_cluster);
}
