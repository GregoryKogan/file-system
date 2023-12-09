#pragma once

#include "Converter/Converter.hpp"
#include "DiskHandler/DiskReader/DiskReader.hpp"
#include "DiskHandler/DiskWriter/DiskWriter.hpp"
#include "FAT/FAT.hpp"
#include "FSMaker/FSMaker.hpp"
#include "FileHandler/FileReader/FileReader.hpp"
#include "FileHandler/FileWriter/FileWriter.hpp"
#include "FileHandler/HandlerBuilder/HandlerBuilder.hpp"
#include "Metadata/Metadata.hpp"
#include "PathResolver/PathResolver.hpp"
#include <cstdint>
#include <sstream>
#include <string>

#include <iostream>

class FileSystem {
  FSMaker fs_maker_;
  FSMaker::Settings settings_ = {};

  DiskReader disk_reader_;
  DiskWriter disk_writer_;

  FAT fat_;

  HandlerBuilder handler_builder_;

  PathResolver path_resolver_;

  std::uint64_t working_dir_cluster_ = 0;

public:
  FileSystem() = default;
  explicit FileSystem(std::string const &path);

  static void make(std::string const &path, FSMaker::Settings const &settings, bool allow_big = false);

  [[nodiscard]] auto get_settings() const noexcept -> FSMaker::Settings const &;

  [[nodiscard]] auto ls(std::string const &path) const -> std::vector<Metadata>;
  [[nodiscard]] auto dirname(std::string const &path) -> std::string;
  [[nodiscard]] auto basename(std::string const &path) -> std::string;
  auto mkdir(std::string const &path) -> void;

  friend auto operator<<(std::ostream &out_stream, FileSystem const &file_system) -> std::ostream &;

private:
  void read_settings();
  [[nodiscard]] auto is_root_dir_created() noexcept -> bool;
  auto create_root_dir() -> void;
  // [[nodiscard]] auto root_dir_size() const noexcept -> std::uint64_t;
  // [[nodiscard]] auto root_dir_file_data() const -> FileData;

  // [[nodiscard]] auto search(std::string const &path) const -> std::optional<FileData>;

  // [[nodiscard]] auto get_parent_dir_data(std::string const &path) -> std::optional<FileData>;
  // auto write_new_dir(FileData const &parent_dir_data, std::string const &dir_name) -> FileData;
  // auto update_parent_dir(FileData const &parent_dir_data, FileData const &new_file_data) -> void;
};