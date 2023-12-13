#include "../src/FileSystem/FileSystem.hpp"
#include <filesystem>
#include <gtest/gtest.h>
#include <sstream>

class CatTest : public testing::Test {
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
  }

  auto TearDown() -> void override { std::filesystem::remove(PATH); }
};

TEST_F(CatTest, CatEmptyFile) {
  file_system_.touch("file1");
  std::ostringstream oss;
  file_system_.cat("file1", oss);
  auto content = oss.str();
  EXPECT_EQ(content.length(), 0);
}

TEST_F(CatTest, CatFile) {
  file_system_.touch("file1");
  auto writer = file_system_.get_writer("file1");
  writer.write(Converter::to_bytes(std::string("Hello World!")));
  std::ostringstream oss;
  file_system_.cat("file1", oss);
  auto content = oss.str();
  EXPECT_EQ(content, "Hello World!");
}

TEST_F(CatTest, CatFileWithNewline) {
  file_system_.touch("file1");
  auto writer = file_system_.get_writer("file1");
  writer.write(Converter::to_bytes(std::string("Hello World!\n")));
  std::ostringstream oss;
  file_system_.cat("file1", oss);
  auto content = oss.str();
  EXPECT_EQ(content, "Hello World!\n");
}

TEST_F(CatTest, CatDirectory) {
  file_system_.mkdir("dir1");
  std::ostringstream oss;
  EXPECT_THROW(file_system_.cat("dir1", oss), std::invalid_argument);
}

TEST_F(CatTest, CatNonExistingFile) {
  std::ostringstream oss;
  EXPECT_THROW(file_system_.cat("file1", oss), std::invalid_argument);
}

TEST_F(CatTest, CatFileInDirectory) {
  file_system_.mkdir("dir1");
  file_system_.touch("dir1/file1");
  auto writer = file_system_.get_writer("dir1/file1");
  writer.write(Converter::to_bytes(std::string("Hello World!")));
  std::ostringstream oss;
  EXPECT_THROW(file_system_.cat("dir1", oss), std::invalid_argument);
}

TEST_F(CatTest, CatFileWithMultipleLines) {
  file_system_.touch("file1");
  auto writer = file_system_.get_writer("file1");
  writer.write_next(Converter::to_bytes(std::string("1 Hello World!\n")));
  writer.write_next(Converter::to_bytes(std::string("2 Hello World!\n")));
  writer.write_next(Converter::to_bytes(std::string("3 Hello World!\n")));
  std::ostringstream oss;
  file_system_.cat("file1", oss);
  auto content = oss.str();
  EXPECT_EQ(content, "1 Hello World!\n2 Hello World!\n3 Hello World!\n");
}