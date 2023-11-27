#include <fstream>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include "file_io/FileReader/FileReader.hpp"
#include "file_io/FileWriter/FileWriter.hpp"

struct FileData {
  std::string name;
  std::string extension;
  std::uint32_t size;
};

auto glue_files(const std::vector<std::string> &files, const std::string &output_file) -> std::vector<FileData> {
  auto ofs = std::make_unique<std::ofstream>(output_file, std::ios::binary);
  FileWriter writer(std::move(ofs), FileHandler::FileOffset{0});

  std::vector<FileData> meta_data;

  const uint32_t BLOCK_SIZE = 100;
  for (const auto &file : files) {
    auto ifs = std::make_unique<std::ifstream>(file, std::ios::binary);
    FileReader reader(std::move(ifs), FileHandler::FileOffset{0}, FileHandler::BlockSize{BLOCK_SIZE});

    while (true) {
      auto block = reader.read_next_block();
      writer.write_next_block(block);
      if (block.size() < reader.block_size()) break;
    }

    std::string file_name =
        file.substr(file.find_last_of('/') + 1, file.find_last_of('.') - file.find_last_of('/') - 1);
    std::string file_extension = file.substr(file.find_last_of('.') + 1);
    meta_data.push_back({file_name, file_extension, reader.handled_size()});
  }

  return meta_data;
}

void split_file(const std::string &file, const std::vector<FileData> &meta_data) {
  auto ifs = std::make_unique<std::ifstream>(file, std::ios::binary);
  FileReader reader(std::move(ifs), FileHandler::FileOffset{0}, FileHandler::BlockSize{1});

  for (const auto &file_meta : meta_data) {
    std::string output_file = "../src/output/" + file_meta.name + "." + file_meta.extension;
    auto ofs = std::make_unique<std::ofstream>(output_file, std::ios::binary);

    FileWriter writer(std::move(ofs), FileHandler::FileOffset{0});
    for (size_t j = 0; j < file_meta.size; ++j) {
      auto block = reader.read_next_block();
      writer.write_next_block(block);
    }
  }
}

auto main() -> int {
  try {
    const std::vector<std::string> FILES = {"../src/assets/AnimeGirls.jpg", "../src/assets/Beeps.mp3",
                                            "../src/assets/Tiger.jpeg"};
    const std::string OUTPUT_FILE = "../src/output/glued.bin";

    auto meta_data = glue_files(FILES, OUTPUT_FILE);
    split_file(OUTPUT_FILE, meta_data);
  } catch (const std::exception &e) { std::cout << e.what() << "\n"; } catch (...) {
    std::cout << "Unknown exception\n";
  }
}