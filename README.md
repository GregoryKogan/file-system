# file-system

![GitHub Workflow Status (with event)](https://img.shields.io/github/actions/workflow/status/GregoryKogan/file-system/cmake-multi-platform.yml)
![](https://tokei.ekzhang.com/b1/github/GregoryKogan/file-system)

Simple implementation of FAT file system

## CLI

### Commands

- `help` - show this message
- `exit` - exit the program
- `clear` - clear the screen
- `fsinfo` - show file system info
- `dirname <path>` - get the directory portion of a pathname
- `basename <path>` - get the filename portion of a pathname
- `pwd` - print current working directory
- `ls [-l]` - list directory contents
- `stat <path>` - print file metadata
- `cat <path>` - print file contents
- `mkdir <path>` - create a directory
- `cd <path>` - change the working directory
- `touch <path>` - create a file
- `rmdir <path>` - remove a directory
- `rm [-r] <path>` - remove directory entries
- `cp [-r] <source> <destination>` - copy files and directories
- `mv [-r] <source> <destination>` - move files and directories
- `import <host_path> <fs_path>` - import a file from the host file system
- `export <fs_path> <host_path>` - export a file to the host file system
