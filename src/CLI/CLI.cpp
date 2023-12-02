#include "CLI.hpp"

#include <utility>

CLI::CLI() {
  const std::string PATH = "test.fs";
  const uint64_t SIZE = 1024;
  const uint64_t CLUSTER_SIZE = 64;

  FileSystem::make(PATH, {SIZE, CLUSTER_SIZE});

  file_system_ = std::make_unique<FileSystem>(PATH);
}

auto CLI::run() -> void {
  std::cout << "Welcome to the File System!\n";
  std::cout << "Type 'help' to see available commands.\n";

  std::string user_input;
  while (true) {
    std::cout << prompt();

    std::getline(std::cin, user_input);

    std::istringstream iss(user_input);

    std::string command;
    iss >> command;

    std::vector<std::string> args;
    std::string arg;
    while (iss >> arg) { args.push_back(arg); }

    if (command == "exit") { break; }

    execute(command, args);
  }
}

auto CLI::prompt() -> std::string { return ">>> "; }

auto CLI::execute(std::string const &command, std::vector<std::string> args) -> void {
  if (command == "help") {
    help();
  } else if (command == "dirname") {
    dirname(std::move(args));
  } else if (command == "basename") {
    basename(std::move(args));
  } else if (command == "ls") {
    ls();
  } else {
    std::cout << "Unknown command. Type 'help' to see available commands.\n";
  }
}

auto CLI::help() -> void {
  std::cout << "Commands:\n";
  std::cout << "-\t'help' - show this message\n";
  std::cout << "-\t'exit' - exit the program\n";
  std::cout << "-\t'dirname <path>' - get the directory portion of a pathname\n";
  std::cout << "-\t'basename <path>' - get the filename portion of a pathname\n";
  std::cout << "-\t'ls' - list directory contents\n";
}

auto CLI::dirname(std::vector<std::string> args) -> void {
  if (args.size() != 1) {
    std::cout << "Wrong number of arguments. Usage: dirname <path>\n";
    return;
  }

  std::cout << file_system_->dirname(args[0]) << '\n';
}

auto CLI::basename(std::vector<std::string> args) -> void {
  if (args.size() != 1) {
    std::cout << "Wrong number of arguments. Usage: basename <path>\n";
    return;
  }

  std::cout << file_system_->basename(args[0]) << '\n';
}

auto CLI::ls() -> void {
  auto files = file_system_->ls("/");
  for (auto const &file : files) { std::cout << file.to_string() << '\n'; }
}
