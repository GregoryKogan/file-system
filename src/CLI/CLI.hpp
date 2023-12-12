#pragma once

#include "../FileSystem/FileSystem.hpp"
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <string>
#include <utility>

class CLI {
  FileSystem file_system_;

public:
  CLI();
  CLI(CLI const &) = delete;
  CLI(CLI &&) = delete;

  ~CLI();

  auto operator=(CLI const &) -> CLI & = delete;
  auto operator=(CLI &&) -> CLI & = delete;

  auto run() -> void;

private:
  [[nodiscard]] auto prompt() -> std::string;
  auto execute(std::string const &command, std::vector<std::string> args) -> void;
  static auto help() -> void;
  static auto clear() -> void;

  auto fsinfo() -> void;
  auto dirname(std::vector<std::string> args) -> void;
  auto basename(std::vector<std::string> args) -> void;
  auto pwd() -> void;
  auto ls(std::vector<std::string> args) -> void;
  auto mkdir(std::vector<std::string> args) -> void;
  auto cd(std::vector<std::string> args) -> void;
  auto touch(std::vector<std::string> args) -> void;
};