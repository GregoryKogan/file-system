#include "../src/FileSystem/FileSystem.hpp"
#include <filesystem>
#include <gtest/gtest.h>

class MvTest : public testing::Test {
protected:
  // NOLINTBEGIN(cppcoreguidelines-non-private-member-variables-in-classes)
  std::string const PATH = "test.fs";
  std::uint64_t const SIZE = 131072;
  std::uint64_t const CLUSTER_SIZE = 256;

  FileSystem file_system_;
  // NOLINTEND(cppcoreguidelines-non-private-member-variables-in-classes)

  auto SetUp() -> void override {
    FileSystem::make(PATH, {SIZE, CLUSTER_SIZE});
    file_system_ = FileSystem(PATH);
    file_system_.mkdir("samples");
    file_system_.cd("samples");
    import_files();
    file_system_.cd("/");
  }

  auto TearDown() -> void override { std::filesystem::remove(PATH); }

private:
  auto import_files() -> void {
    import_file("../assets/short_sample.txt", "short.txt");
    import_file("../assets/long_sample.txt", "long.txt");
  }

  auto import_file(std::string const &host_fs_file_path, std::string const &file_system_path) -> void {
    std::ifstream file_to_import(host_fs_file_path);
    if (!file_to_import.is_open()) throw std::runtime_error("Cannot open file " + std::string(host_fs_file_path));
    file_system_.import_file(file_to_import, file_system_path);
    file_to_import.close();
  }
};

TEST_F(MvTest, MoveShortFile) {
  std::ostringstream original;
  file_system_.cat("samples/short.txt", original);

  file_system_.mkdir("dir");
  file_system_.mv("samples/short.txt", "dir/short.txt");

  std::ostringstream moved;
  file_system_.cat("dir/short.txt", moved);

  EXPECT_EQ(original.str(), moved.str());
}

TEST_F(MvTest, MoveLongFile) {
  std::ostringstream original;
  file_system_.cat("samples/long.txt", original);

  file_system_.mkdir("dir");
  file_system_.mv("samples/long.txt", "dir/long.txt");

  std::ostringstream moved;
  file_system_.cat("dir/long.txt", moved);

  EXPECT_EQ(original.str(), moved.str());
}

TEST_F(MvTest, MoveDirectory) {
  file_system_.mkdir("dir");
  file_system_.mv("samples", "dir/samples", true);

  auto dir_list = file_system_.ls("dir");
  ASSERT_EQ(dir_list.size(), 1);
  EXPECT_EQ(dir_list[0].get_name(), "samples");

  auto samples_list = file_system_.ls("dir/samples");
  ASSERT_EQ(samples_list.size(), 2);
  EXPECT_EQ(samples_list[0].get_name(), "short.txt");
  EXPECT_EQ(samples_list[1].get_name(), "long.txt");
}

TEST_F(MvTest, MoveFileRecursively) {
  std::ostringstream original;
  file_system_.cat("samples/long.txt", original);

  file_system_.mkdir("dir");
  file_system_.mv("samples/long.txt", "dir/long.txt", true);

  std::ostringstream moved;
  file_system_.cat("dir/long.txt", moved);

  EXPECT_EQ(original.str(), moved.str());
}

TEST_F(MvTest, MoveEmptyDirectoryNonRecursively) {
  file_system_.mkdir("dir");
  file_system_.mv("dir", "dir2");

  auto list = file_system_.ls(".");
  ASSERT_EQ(list.size(), 2);
  EXPECT_EQ(list[0].get_name(), "samples");
  EXPECT_EQ(list[1].get_name(), "dir2");
}

TEST_F(MvTest, MoveEmptyDirectory) {
  file_system_.mkdir("dir");
  file_system_.mv("dir", "dir2", true);

  auto list = file_system_.ls(".");
  ASSERT_EQ(list.size(), 2);
  EXPECT_EQ(list[0].get_name(), "samples");
  EXPECT_EQ(list[1].get_name(), "dir2");
}

TEST_F(MvTest, MoveDirectoryNonRecursively) {
  file_system_.mkdir("dir");
  EXPECT_THROW(file_system_.mv("samples", "dir/samples"), std::invalid_argument);
}

TEST_F(MvTest, MoveNonExistingFile) {
  EXPECT_THROW(file_system_.mv("samples/non_existing.txt", "non_existing.txt"), std::invalid_argument);
}

TEST_F(MvTest, MoveFileToNonExistingDirectory) {
  EXPECT_THROW(file_system_.mv("samples/short.txt", "non_existing/short.txt"), std::invalid_argument);
}