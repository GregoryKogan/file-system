#pragma once

#include "Converter/Converter.hpp"
#include "DiskHandler/DiskReader/DiskReader.hpp"
#include "DiskHandler/DiskWriter/DiskWriter.hpp"
#include "FAT/FAT.hpp"
#include "FSMaker/FSMaker.hpp"
#include <cstdint>
#include <sstream>
#include <string>

#include <iostream>

class FileSystem : public FSMaker {
  std::shared_ptr<DiskReader> fs_reader_;
  Settings settings_ = {0, 0};
  std::unique_ptr<FAT> fat_;

public:
  explicit FileSystem(std::string const &path);

  [[nodiscard]] auto get_settings() const noexcept -> Settings const &;

  friend auto operator<<(std::ostream &out_stream, FileSystem const &file_system) -> std::ostream &;

private:
  void read_settings();
};