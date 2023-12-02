#pragma once

#include <optional>
#include <string>
#include <vector>

#include <iostream>

#include "../Directory/Directory.hpp"
#include "../DiskHandler/DiskReader/DiskReader.hpp"
#include "../FAT/FAT.hpp"
#include "../FileData/FileData.hpp"
#include "../FileHandler/FileReader/FileReader.hpp"

class PathResolver {
  std::string delimiter_;
  std::shared_ptr<DiskReader> disk_reader_;
  std::shared_ptr<FAT> fat_;
  std::uint64_t cluster_size_;

public:
  PathResolver(std::string delimiter, std::shared_ptr<DiskReader> disk_reader, std::shared_ptr<FAT> fat,
               std::uint64_t cluster_size);

  [[nodiscard]] auto delimiter() const -> std::string const &;

  [[nodiscard]] auto search(std::string const &path, FileData const &working_dir, FileData const &root_dir) const
      -> std::optional<FileData>;
  static auto parse(std::string const &path, std::string const &delimiter) -> std::vector<std::string>;
  [[nodiscard]] static auto dirname(std::string const &path, std::string const &delimiter) -> std::string;
  [[nodiscard]] static auto basename(std::string const &path, std::string const &delimiter) -> std::string;

private:
  [[nodiscard]] auto get_file(std::vector<std::string> const &path_tokens, FileData const &file_data) const
      -> std::optional<FileData>;
};