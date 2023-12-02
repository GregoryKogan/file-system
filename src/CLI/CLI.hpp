#pragma once

#include "../FileSystem/FileSystem.hpp"
#include <cstdlib>
#include <iostream>
#include <string>

class CLI {
  std::unique_ptr<FileSystem> file_system_;

public:
  CLI();

  auto run() -> void;

private:
  [[nodiscard]] static auto prompt() -> std::string;
  auto execute(std::string const &command, std::vector<std::string> args) -> void;
  static auto help() -> void;
  static auto clear() -> void;

  auto dirname(std::vector<std::string> args) -> void;
  auto basename(std::vector<std::string> args) -> void;
  auto ls(std::vector<std::string> args) -> void;
};