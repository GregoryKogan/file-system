# file-system

![Lines of code](https://tokei.ekzhang.com/b1/github/GregoryKogan/file-system)
![GitHub Workflow Status (with event)](https://img.shields.io/github/actions/workflow/status/GregoryKogan/file-system/cmake-multi-platform.yml)

Simple implementation of FAT file system

## CLI

### Commands

- `help` - show help message
- `exit` - exit the program
- `clear` - clear the screen
- `fsinfo` - show file system info
- `dirname <path>` - get the directory portion of a pathname
- `basename <path>` - get the filename portion of a pathname
- `ls [-l]` - list directory contents
- `mkdir <path>` - create a directory
- `cd <path>` - change working directory
- `pwd` - print working directory
- `touch <path>` - create a file
- `cat <path>` - print file contents
- `rm <path>` - remove a file or directory
- `rmdir <path>` - remove a directory
- `mv <path> <path>` - move a file or directory
- `cp <path> <path>` - copy a file or directory
- `import <path> <path>` - import a file or directory from host file system
- `export <path> <path>` - export a file or directory to host file system
- `stat <path>` - show file or directory info
- `find <path> <name>` - find a file or directory by name
