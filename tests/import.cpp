#include "../src/FileSystem/FileSystem.hpp"
#include <filesystem>
#include <gtest/gtest.h>

class ImportTest : public testing::Test {
protected:
  // NOLINTBEGIN(cppcoreguidelines-non-private-member-variables-in-classes)
  std::string const PATH = "test.fs";
  std::uint64_t const SIZE = 32768;
  std::uint64_t const CLUSTER_SIZE = 64;

  FileSystem file_system_;
  // NOLINTEND(cppcoreguidelines-non-private-member-variables-in-classes)

  auto SetUp() -> void override {
    FileSystem::make(PATH, {SIZE, CLUSTER_SIZE});
    file_system_ = FileSystem(PATH);
  }

  auto TearDown() -> void override { std::filesystem::remove(PATH); }
};

TEST_F(ImportTest, ImportFile) {
  const auto *const file_name = "test.txt";
  const auto *const file_content =
      "Hello World! This is some sample text. It is not very long, but it is long enough to test the file system.";

  std::ofstream file(file_name);
  file << file_content;
  file.close();

  std::ifstream file_to_import(file_name);
  file_system_.import_file(file_to_import, file_name);
  file_to_import.close();

  std::ostringstream oss;
  file_system_.cat(file_name, oss);
  EXPECT_EQ(std::string(file_content), oss.str());
}

TEST_F(ImportTest, AlreadyExists) {
  file_system_.touch("test.txt");

  const auto *const file_name = "test.txt";
  const auto *const file_content =
      "Hello World! This is some sample text. It is not very long, but it is long enough to test the file system.";

  std::ofstream file(file_name);
  file << file_content;
  file.close();

  std::ifstream file_to_import(file_name);
  EXPECT_THROW(file_system_.import_file(file_to_import, file_name), std::invalid_argument);
  file_to_import.close();
}

TEST_F(ImportTest, ImportToDir) {
  const auto *const file_name = "test.txt";
  const auto *const file_content =
      "Hello World! This is some sample text. It is not very long, but it is long enough to test the file system.";

  std::ofstream file(file_name);
  file << file_content;
  file.close();

  file_system_.mkdir("dir");
  std::ifstream file_to_import("test.txt");
  EXPECT_THROW(file_system_.import_file(file_to_import, "dir"), std::invalid_argument);
  file_to_import.close();
}

TEST_F(ImportTest, ImportShortRealFile) {
  const auto *const host_fs_file_path = "../assets/short_sample.txt";

  std::ifstream file_to_import(host_fs_file_path);
  if (!file_to_import.is_open()) throw std::runtime_error("Cannot open file " + std::string(host_fs_file_path));
  file_system_.import_file(file_to_import, "sample.txt");
  file_to_import.close();

  std::ifstream host_fs_file(host_fs_file_path);
  if (!host_fs_file.is_open()) throw std::runtime_error("Cannot open file " + std::string(host_fs_file_path));
  std::string const host_fs_file_content((std::istreambuf_iterator<char>(host_fs_file)),
                                         std::istreambuf_iterator<char>());

  std::ostringstream oss;
  file_system_.cat("sample.txt", oss);

  EXPECT_EQ(host_fs_file_content, oss.str());
  EXPECT_EQ(oss.str(), "Good news! Import works!");
  host_fs_file.close();
}

TEST_F(ImportTest, ImportLongRealFile) {
  const auto *const host_fs_file_path = "../assets/long_sample.txt";

  std::ifstream file_to_import(host_fs_file_path);
  if (!file_to_import.is_open()) throw std::runtime_error("Cannot open file " + std::string(host_fs_file_path));
  file_system_.import_file(file_to_import, "sample.txt");
  file_to_import.close();

  std::ifstream host_fs_file(host_fs_file_path);
  if (!host_fs_file.is_open()) throw std::runtime_error("Cannot open file " + std::string(host_fs_file_path));
  std::string const host_fs_file_content((std::istreambuf_iterator<char>(host_fs_file)),
                                         std::istreambuf_iterator<char>());

  std::ostringstream oss;
  file_system_.cat("sample.txt", oss);

  EXPECT_EQ(host_fs_file_content, oss.str());
  host_fs_file.close();
}