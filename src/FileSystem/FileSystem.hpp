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
#include <algorithm>
#include <cstdint>
#include <sstream>
#include <string>
#include <utility>

#include <iostream>

class FileSystem {
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

  static auto make(std::string const &path, FSMaker::Settings const &settings, bool allow_big = false) -> void;

  [[nodiscard]] auto get_settings() const noexcept -> FSMaker::Settings const &;

  [[nodiscard]] auto dirname(std::string const &path) const -> std::string;
  [[nodiscard]] auto basename(std::string const &path) const -> std::string;
  [[nodiscard]] auto get_reader(std::string const &path) const -> FileReader;
  [[nodiscard]] auto get_writer(std::string const &path) -> FileWriter;
  [[nodiscard]] auto pwd() const -> std::string;
  [[nodiscard]] auto ls(std::string const &path) const -> std::vector<Metadata>;
  [[nodiscard]] auto stat(std::string const &path) const -> Metadata;
  auto cd(std::string const &path) -> void;
  auto cat(std::string const &path, std::ostream &out_stream) const -> void;
  auto mkdir(std::string const &path) -> void;
  auto touch(std::string const &path) -> void;
  auto rmdir(std::string const &path) -> void;
  auto rm(std::string const &path, bool recursive = false) -> void;
  auto cp(std::string const &source, std::string const &destination, bool recursive = false) -> void;
  auto mv(std::string const &source, std::string const &destination, bool recursive = false) -> void;
  auto import_file(std::istream &in_stream, std::string const &path) -> void;
  auto export_file(std::string const &path, std::ostream &out_stream) const -> void;

  friend auto operator<<(std::ostream &out_stream, FileSystem const &file_system) -> std::ostream &;

private:
  [[nodiscard]] auto check_signature() -> bool;
  auto read_settings() -> void;
  [[nodiscard]] auto is_root_dir_created() noexcept -> bool;
  auto create_root_dir() -> void;
  [[nodiscard]] auto read_dir(std::uint64_t cluster) const -> Directory;
  [[nodiscard]] auto get_metadata_from_clusters(const std::vector<std::uint64_t> &clusters) const
      -> std::vector<Metadata>;
  [[nodiscard]] auto search(std::string const &path) const -> std::optional<std::uint64_t>;
  [[nodiscard]] auto does_exist(std::string const &path) const -> bool;
  [[nodiscard]] auto does_file_exist(std::string const &path) const -> bool;
  [[nodiscard]] auto does_dir_exist(std::string const &path) const -> bool;
  [[nodiscard]] auto alloc_new_dir(std::string const &name, std::uint64_t parent_cluster) -> std::uint64_t;
  auto add_file_to_dir(std::uint64_t parent_cluster, std::uint64_t child_cluster) const -> void;
  auto remove_file_from_dir(std::uint64_t parent_cluster, std::uint64_t child_cluster) -> void;
  auto overwrite_file(std::uint64_t cluster, Metadata old_meta, std::vector<std::byte> const &bytes) -> void;
  auto rmfile(std::string const &path) -> void;
  auto rm_recursive(std::string const &path) -> void;
  auto shallow_copy(std::string const &source, std::string const &destination) -> void;
  auto deep_copy(std::string const &source, std::string const &destination) -> void;
};