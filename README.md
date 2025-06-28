# FAT File System Emulator

![GitHub License](https://img.shields.io/github/license/GregoryKogan/file-system)
![GitHub Workflow Status (with event)](https://img.shields.io/github/actions/workflow/status/GregoryKogan/file-system/cmake-multi-platform.yml?logo=github&label=tests)
![](https://tokei.ekzhang.com/b1/github/GregoryKogan/file-system)

## Introduction

This project is a custom-built file system emulator written in C++, simulating core file system operations like creating, reading, writing, copying, and removing files and directories. It offers a fully functional Command-Line Interface (CLI) for interacting with the virtual file system. This project provides an educational and testable environment to understand file system behaviors and operations.

## Table of Contents

* [Introduction](#introduction)
* [Features](#features)
* [Installation](#installation)
* [Usage](#usage)
* [Available Commands](#available-commands)
* [Project Structure](#project-structure)
* [Dependencies](#dependencies)
* [Testing](#testing)
* [Examples](#examples)
* [Troubleshooting](#troubleshooting)
* [License](#license)

## Features

✅ Custom file system with cluster-based storage  
✅ FAT (File Allocation Table) management  
✅ Directory structure and metadata handling  
✅ File read/write, import/export, and copy/move functionalities  
✅ CLI for interactive user commands  
✅ Unit tests for critical components  
✅ Cross-platform build via CMake and GitHub Actions  

## Installation

### Prerequisites

* CMake (version 3.15 or later recommended)
* C++17 compatible compiler (GCC, Clang, etc.)
* Git

### Build Steps

```bash
git clone <repository-url>
cd <repository-root>
mkdir build && cd build
cmake ..
make
```

To run tests:

```bash
ctest --output-on-failure
```

## Usage

After building, run the CLI:

```bash
./cli
```

Example interaction:

```bash
Welcome to the File System!
Type 'help' to see available commands.
```

## Available Commands

* `help` — Show help message
* `exit` — Exit the program
* `clear` — Clear the terminal screen
* `makefs <path> <size> <cluster_size>` — Create a new file system
* `openfs <path>` — Open an existing file system
* `fsinfo` — Display file system information
* `pwd` — Show current working directory
* `ls [-l] <path>` — List directory contents
* `mkdir <path>` — Create a directory
* `cd <path>` — Change directory
* `touch <path>` — Create an empty file
* `cat <path>` — Print file contents
* `stat <path>` — Show file metadata
* `rmdir <path>` — Remove directory
* `rm [-r] <path>` — Remove files or directories
* `cp [-r] <src> <dst>` — Copy files or directories
* `mv [-r] <src> <dst>` — Move files or directories
* `import <host_path> <fs_path>` — Import file from host
* `export <fs_path> <host_path>` — Export file to host

## Project Structure

```
.github/workflows/      # CI/CD configuration for multi-platform builds  
assets/                 # Sample files for import/export testing  
src/                    # Core source code  
 ├── CLI/               # Command-Line Interface implementation  
 ├── FileSystem/        # File system components (FAT, Metadata, File Handlers, etc.)  
tests/                  # Unit and integration tests  
CMakeLists.txt          # Build configuration  
README.md               # Project documentation  
LICENSE                 # License information  
```

## Dependencies

* C++17 Standard Library
* CMake for building
* No external runtime dependencies required

## Testing

Run tests after building:

```bash
ctest --output-on-failure
```

Unit tests cover:

* Directory and file operations
* FAT cluster management
* File read/write logic
* CLI command correctness

## Examples

Create and use a file system:

```bash
makefs myfs.fs 1048576 256
openfs myfs.fs
mkdir /docs
touch /docs/readme.txt
import hostfile.txt /docs/readme.txt
ls /docs
cat /docs/readme.txt
export /docs/readme.txt exported.txt
```

## Troubleshooting

* **Cannot open file system**: Ensure the `.fs` file exists and is accessible
* **Command not recognized**: Use `help` to verify command syntax
* **Build issues**: Confirm correct compiler and CMake version installed

## License

This project is licensed under the terms of the [LICENSE](LICENSE) file included in the repository.
