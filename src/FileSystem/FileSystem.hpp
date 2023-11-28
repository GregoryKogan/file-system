#pragma once

#include "Converter/Converter.hpp"
#include "FSMaker/FSMaker.hpp"
#include "file_io/FileReader/FileReader.hpp"
#include "file_io/FileWriter/FileWriter.hpp"
#include <cstdint>
#include <string>

#include <iostream>

class FileSystem : public FSMaker {
  std::unique_ptr<FileReader> fs_reader_;
  Settings settings_ = {0, 0};

public:
  FileSystem() = default;

  void open_fs(std::string const &path);

  [[nodiscard]] auto get_settings() const noexcept -> Settings const &;

private:
  void read_settings();
};