#pragma once

#include "../FAT/FAT.hpp"
#include "../FileData/FileData.hpp"

class FileHandler {
  std::shared_ptr<FAT> fat_;
  std::uint64_t cluster_size_;
  FileData file_data_;
  std::uint64_t offset_;
  std::uint64_t handled_size_ = 0;

public:
  struct FileOffset;

  FileHandler(FileData file_data, FileOffset offset, std::shared_ptr<FAT> fat, std::uint64_t cluster_size);

  [[nodiscard]] auto offset() const noexcept -> std::uint64_t;
  [[nodiscard]] auto handled_size() const noexcept -> std::uint64_t;

  void set_offset(FileOffset offset);

protected:
  [[nodiscard]] auto fat() const noexcept -> std::shared_ptr<FAT>;
  [[nodiscard]] auto cluster_size() const noexcept -> std::uint64_t;
  [[nodiscard]] auto file_data() const noexcept -> FileData;
  void increase_handled_size(std::uint64_t size) noexcept;
};

struct FileHandler::FileOffset {
  std::uint64_t value;

  explicit FileOffset(std::uint64_t value);
};