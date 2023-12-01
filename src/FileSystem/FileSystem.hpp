#pragma once

#include "Converter/Converter.hpp"
#include "DiskHandler/DiskReader/DiskReader.hpp"
#include "DiskHandler/DiskWriter/DiskWriter.hpp"
#include "FAT/FAT.hpp"
#include "FSMaker/FSMaker.hpp"
#include "FileData/FileData.hpp"
#include "FileHandler/FileReader/FileReader.hpp"
#include "FileHandler/FileWriter/FileWriter.hpp"
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
  std::uint64_t root_dir_size_;

public:
  explicit FileSystem(std::string const &path);

  static void make(std::string const &path, FSMaker::Settings const &settings, bool allow_big = false);

  [[nodiscard]] auto get_settings() const noexcept -> FSMaker::Settings const &;

  [[nodiscard]] auto ls(std::string const &path) const -> std::vector<FileData>;

  friend auto operator<<(std::ostream &out_stream, FileSystem const &file_system) -> std::ostream &;

private:
  void read_settings();
};