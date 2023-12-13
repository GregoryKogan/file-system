#include "../src/FileSystem/FileSystem.hpp"
#include <filesystem>
#include <gtest/gtest.h>

class ReaderWriterTest : public testing::Test {
protected:
  // NOLINTBEGIN(cppcoreguidelines-non-private-member-variables-in-classes)
  std::string const PATH = "test.fs";
  std::uint64_t const SIZE = 2048;
  std::uint64_t const CLUSTER_SIZE = 64;

  FileSystem file_system_;
  // NOLINTEND(cppcoreguidelines-non-private-member-variables-in-classes)

  auto SetUp() -> void override {
    FileSystem::make(PATH, {SIZE, CLUSTER_SIZE});
    file_system_ = FileSystem(PATH);
    file_system_.touch("file");
  }

  auto TearDown() -> void override { std::filesystem::remove(PATH); }
};

TEST_F(ReaderWriterTest, WriteRead) {
  auto writer = file_system_.get_writer("file");
  auto reader = file_system_.get_reader("file");

  std::string const data = "Hello, World!";
  writer.set_offset(0);
  writer.write(Converter::to_bytes(data));

  reader.set_offset(0);
  reader.set_block_size(data.size());
  auto const read_data = Converter::to_string(reader.read());

  EXPECT_EQ(data, read_data);
}

TEST_F(ReaderWriterTest, WriteReadMultiple) {
  auto writer = file_system_.get_writer("file");
  auto reader = file_system_.get_reader("file");

  std::string const data = "Hello, World!";
  writer.set_offset(0);
  writer.write(Converter::to_bytes(data));

  std::string const data2 = "Hello, World!2";
  writer.set_offset(data.size());
  writer.write(Converter::to_bytes(data2));

  reader.set_offset(0);
  reader.set_block_size(data.size());
  auto const read_data = Converter::to_string(reader.read());

  EXPECT_EQ(data, read_data);

  reader.set_offset(data.size());
  reader.set_block_size(data2.size());
  auto const read_data2 = Converter::to_string(reader.read());

  EXPECT_EQ(data2, read_data2);
}

TEST_F(ReaderWriterTest, ReadPartial) {
  auto writer = file_system_.get_writer("file");
  auto reader = file_system_.get_reader("file");

  std::string const data = "Hello, World!";
  writer.set_offset(0);
  writer.write(Converter::to_bytes(data));

  std::uint64_t const offset = 3;
  std::uint64_t const size = 5;
  reader.set_offset(offset);
  reader.set_block_size(size);
  auto const read_data = Converter::to_string(reader.read());

  EXPECT_EQ(data.substr(offset, size), read_data);
}

TEST_F(ReaderWriterTest, OverwritePartial) {
  auto writer = file_system_.get_writer("file");
  auto reader = file_system_.get_reader("file");

  writer.set_offset(0);
  writer.write(Converter::to_bytes(std::string("Hello, World!")));
  writer.set_offset(std::string("Hello, ").length());
  writer.write(Converter::to_bytes(std::string("James")));

  reader.set_offset(0);
  reader.set_block_size(std::string("Hello, James!").length());
  auto const read_data = Converter::to_string(reader.read());

  EXPECT_EQ(std::string("Hello, James!"), read_data);
}

TEST_F(ReaderWriterTest, MultipleLines) {
  auto writer = file_system_.get_writer("file");
  auto reader = file_system_.get_reader("file");

  writer.set_offset(0);
  writer.write_next(Converter::to_bytes(std::string("1 Hello, World!\n")));
  writer.write_next(Converter::to_bytes(std::string("2 Hello, World!\n")));
  writer.write_next(Converter::to_bytes(std::string("3 Hello, World!\n")));

  reader.set_offset(0);
  std::uint64_t const random_block_size = 7;
  reader.set_block_size(random_block_size);

  std::vector<std::byte> data;
  std::vector<std::byte> cur_block = reader.read_next();
  while (!cur_block.empty()) {
    data.insert(data.end(), cur_block.begin(), cur_block.end());
    cur_block = reader.read_next();
  }

  EXPECT_EQ(Converter::to_string(data), std::string("1 Hello, World!\n2 Hello, World!\n3 Hello, World!\n"));
}