#include <fstream>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include "FileSystem/FileSystem.hpp"

auto main() -> int { FileSystem::make_fs("test.fs", FileSystem::Settings{64, 1024}); }
