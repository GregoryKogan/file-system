#include "CLI.hpp"

CLI::CLI() {
  const uint64_t SIZE = 1024;
  const uint64_t CLUSTER_SIZE = 64;

  FileSystem::make("cli.fs", {SIZE, CLUSTER_SIZE});
  file_system_ = FileSystem("cli.fs");
}

CLI::~CLI() { std::filesystem::remove("cli.fs"); }

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

auto CLI::prompt() -> std::string { return file_system_.pwd() + " $ "; }

auto CLI::execute(std::string const &command, std::vector<std::string> args) -> void {
  if (command == "help") {
    help();
  } else if (command == "clear") {
    clear();
  } else if (command == "fsinfo") {
    fsinfo();
  } else if (command == "dirname") {
    dirname(std::move(args));
  } else if (command == "basename") {
    basename(std::move(args));
  } else if (command == "pwd") {
    pwd();
  } else if (command == "cat") {
    cat(std::move(args));
  } else if (command == "ls") {
    ls(std::move(args));
  } else if (command == "mkdir") {
    mkdir(std::move(args));
  } else if (command == "cd") {
    cd(std::move(args));
  } else if (command == "touch") {
    touch(std::move(args));
  } else if (command == "rmdir") {
    rmdir(std::move(args));
  } else if (command == "rm") {
    rm(std::move(args));
  } else if (command == "import") {
    import_file(std::move(args));
  } else if (command == "export") {
    export_file(std::move(args));
  } else {
    std::cout << "Unknown command. Type 'help' to see available commands.\n";
  }
}

auto CLI::help() -> void {
  std::cout << "Commands:\n";
  std::cout << "-\t'help' - show this message\n";
  std::cout << "-\t'exit' - exit the program\n";
  std::cout << "-\t'clear' - clear the screen\n";
  std::cout << "-\t'fsinfo' - show file system info\n";
  std::cout << "-\t'dirname <path>' - get the directory portion of a pathname\n";
  std::cout << "-\t'basename <path>' - get the filename portion of a pathname\n";
  std::cout << "-\t'pwd' - print current working directory\n";
  std::cout << "-\t'ls [-l]' - list directory contents\n";
  std::cout << "-\t'cat <path>' - print file contents\n";
  std::cout << "-\t'mkdir <path>' - create a directory\n";
  std::cout << "-\t'cd <path>' - change the working directory\n";
  std::cout << "-\t'touch <path>' - create a file\n";
  std::cout << "-\t'rmdir <path>' - remove a directory\n";
  std::cout << "-\t'rm [-r] <path>' - remove directory entries\n";
  std::cout << "-\t'import <host_path> <fs_path>' - import a file from the host file system\n";
  std::cout << "-\t'export <fs_path> <host_path>' - export a file to the host file system\n";
}

auto CLI::clear() -> void {
#ifdef WINDOWS
  std::system("cls");
#else
  // Assume POSIX
  if (std::system("clear") != 0) { std::cout << "Error while clearing the screen\n"; }
#endif
}

auto CLI::fsinfo() -> void { std::cout << file_system_ << '\n'; }

auto CLI::dirname(std::vector<std::string> args) -> void {
  if (args.size() != 1) {
    std::cout << "Wrong number of arguments. Usage: dirname <path>\n";
    return;
  }

  std::cout << file_system_.dirname(args[0]) << '\n';
}

auto CLI::basename(std::vector<std::string> args) -> void {
  if (args.size() != 1) {
    std::cout << "Wrong number of arguments. Usage: basename <path>\n";
    return;
  }

  std::cout << file_system_.basename(args[0]) << '\n';
}

auto CLI::pwd() -> void { std::cout << file_system_.pwd() << '\n'; }

auto CLI::ls(std::vector<std::string> args) -> void {
  bool verbose = false;

  std::string path;
  std::vector<Metadata> files;

  if (args.empty()) {
    path = ".";
  } else if (args.size() == 1) {
    if (args[0] == "-l") {
      verbose = true;
      path = ".";
    } else {
      path = args[0];
    }
  } else if (args.size() == 2) {
    if (args[1] == "-l") {
      verbose = true;
      path = args[0];
    } else {
      std::cout << "Wrong arguments. Usage: ls [-l] <path>\n";
      return;
    }
  } else {
    std::cout << "Wrong number of arguments. Usage: ls [-l] <path>\n";
    return;
  }

  files = file_system_.ls(path);

  std::cout << file_system_.basename(path) << "\n";
  for (auto const &file : files) { std::cout << "|----" << Metadata::to_string(file, verbose) << '\n'; }
}

auto CLI::cat(std::vector<std::string> args) -> void {
  if (args.size() != 1) {
    std::cout << "Wrong number of arguments. Usage: cat <path>\n";
    return;
  }

  file_system_.cat(args[0], std::cout);
  std::cout << '\n';
}

auto CLI::mkdir(std::vector<std::string> args) -> void {
  if (args.size() != 1) {
    std::cout << "Wrong number of arguments. Usage: mkdir <path>\n";
    return;
  }

  file_system_.mkdir(args[0]);
}

auto CLI::cd(std::vector<std::string> args) -> void {
  if (args.size() != 1) {
    std::cout << "Wrong number of arguments. Usage: cd <path>\n";
    return;
  }

  file_system_.cd(args[0]);
}

auto CLI::touch(std::vector<std::string> args) -> void {
  if (args.size() != 1) {
    std::cout << "Wrong number of arguments. Usage: touch <path>\n";
    return;
  }

  file_system_.touch(args[0]);
}

auto CLI::rmdir(std::vector<std::string> args) -> void {
  if (args.size() != 1) {
    std::cout << "Wrong number of arguments. Usage: rmdir <path>\n";
    return;
  }

  file_system_.rmdir(args[0]);
}

auto CLI::rm(std::vector<std::string> args) -> void {
  if (args.size() != 1 && args.size() != 2) {
    std::cout << "Wrong number of arguments. Usage: rm [-r] <path>\n";
    return;
  }

  bool recursive = false;
  std::string path;

  if (args.size() == 1) {
    path = args[0];
  } else if (args.size() == 2) {
    if (args[0] == "-r") {
      recursive = true;
      path = args[1];
    } else {
      std::cout << "Wrong arguments. Usage: rm [-r] <path>\n";
      return;
    }
  }

  file_system_.rm(path, recursive);
}

auto CLI::import_file(std::vector<std::string> args) -> void {
  if (args.size() != 2) {
    std::cout << "Wrong number of arguments. Usage: import <host_path> <fs_path>\n";
    return;
  }

  std::ifstream in_stream(args[0], std::ios::binary);
  if (!in_stream.is_open()) {
    std::cout << "Error while opening the file\n";
    return;
  }

  file_system_.import_file(in_stream, args[1]);
  in_stream.close();
}

auto CLI::export_file(std::vector<std::string> args) -> void {
  if (args.size() != 2) {
    std::cout << "Wrong number of arguments. Usage: export <fs_path> <host_path>\n";
    return;
  }

  std::ofstream out_stream(args[1], std::ios::binary);
  if (!out_stream.is_open()) {
    std::cout << "Error while opening the file\n";
    return;
  }

  try {
    file_system_.export_file(args[0], out_stream);
    out_stream.close();
  } catch (const std::exception &e) {
    out_stream.close();
    std::remove(args[1].c_str());
    throw;
  }
}
