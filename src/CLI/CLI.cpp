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

    try {
      execute(command, args);
    } catch (std::exception const &e) { std::cout << e.what() << '\n'; }
  }
}

auto CLI::prompt() -> std::string { return ">>> "; }

auto CLI::execute(std::string const &command, std::vector<std::string> args) -> void {
  if (command == "help") {
    help();
  } else if (command == "clear") {
    clear();
  } else if (command == "dirname") {
    dirname(std::move(args));
  } else if (command == "basename") {
    basename(std::move(args));
  } else if (command == "ls") {
    ls(std::move(args));
  } else if (command == "mkdir") {
    mkdir(std::move(args));
  } else {
    std::cout << "Unknown command. Type 'help' to see available commands.\n";
  }
}

auto CLI::help() -> void {
  std::cout << "Commands:\n";
  std::cout << "-\t'help' - show this message\n";
  std::cout << "-\t'exit' - exit the program\n";
  std::cout << "-\t'clear' - clear the screen\n";
  std::cout << "-\t'dirname <path>' - get the directory portion of a pathname\n";
  std::cout << "-\t'basename <path>' - get the filename portion of a pathname\n";
  std::cout << "-\t'ls [-l]' - list directory contents\n";
  std::cout << "-\t'mkdir <path>' - create a directory\n";
}

auto CLI::clear() -> void {
#ifdef WINDOWS
  std::system("cls");
#else
  // Assume POSIX
  if (std::system("clear") != 0) { std::cout << "Error while clearing the screen\n"; }
#endif
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

auto CLI::ls(std::vector<std::string> args) -> void {
  bool verbose = false;
  if (args.size() == 1 && args[0] == "-l") {
    verbose = true;
  } else if (!args.empty()) {
    std::cout << "Wrong number of arguments. Usage: ls [-l]\n";
    return;
  }

  auto files = file_system_->ls("/");
  for (auto const &file : files) { std::cout << file.to_string(verbose) << '\n'; }
}

auto CLI::mkdir(std::vector<std::string> args) -> void {
  if (args.size() != 1) {
    std::cout << "Wrong number of arguments. Usage: mkdir <path>\n";
    return;
  }

  file_system_->mkdir(args[0]);
}
