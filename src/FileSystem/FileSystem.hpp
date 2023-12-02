#pragma once

#include "Converter/Converter.hpp"
#include "DiskHandler/DiskReader/DiskReader.hpp"
#include "DiskHandler/DiskWriter/DiskWriter.hpp"
#include "FAT/FAT.hpp"
#include "FSMaker/FSMaker.hpp"
#include "FileData/FileData.hpp"
#include "FileHandler/FileReader/FileReader.hpp"
#include "FileHandler/FileWriter/FileWriter.hpp"
#include "PathResolver/PathResolver.hpp"
#include <cstdint>
#include <sstream>
#include <string>

#include <iostream>

class FileSystem {
  FSMaker fs_maker_;
  FSMaker::Settings settings_ = {};

  std::shared_ptr<DiskReader> disk_reader_;
  std::shared_ptr<DiskWriter> disk_writer_;

  std::shared_ptr<FAT> fat_;

  std::unique_ptr<PathResolver> path_resolver_;

  std::shared_ptr<FileData> working_dir_;

public:
  explicit FileSystem(std::string const &path);

  static void make(std::string const &path, FSMaker::Settings const &settings, bool allow_big = false);

  [[nodiscard]] auto get_settings() const noexcept -> FSMaker::Settings const &;

  [[nodiscard]] auto ls(std::string const &path) const -> std::vector<FileData>;
  [[nodiscard]] auto dirname(std::string const &path) -> std::string;
  [[nodiscard]] auto basename(std::string const &path) -> std::string;

  friend auto operator<<(std::ostream &out_stream, FileSystem const &file_system) -> std::ostream &;

private:
  void read_settings();
  [[nodiscard]] auto is_root_dir_created() const noexcept -> bool;
  auto create_root_dir() -> void;
  [[nodiscard]] auto root_dir_size() const noexcept -> std::uint64_t;
  [[nodiscard]] auto root_dir_file_data() const -> FileData;
};