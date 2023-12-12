#include "../src/FileSystem/FileSystem.hpp"
#include <filesystem>
#include <gtest/gtest.h>

class RmdirTest : public testing::Test {
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
    file_system_.mkdir("dir1");
    file_system_.mkdir("dir2");
    file_system_.mkdir("dir1/dir3");
  }

  auto TearDown() -> void override { std::filesystem::remove(PATH); }
};

TEST_F(RmdirTest, RemoveEmptyDirectory) {
  file_system_.rmdir("dir2");

  auto list = file_system_.ls("/");
  ASSERT_EQ(list.size(), 1);
  ASSERT_EQ(list[0].get_name(), "dir1");
}

TEST_F(RmdirTest, RemoveNestedEmptyDirectory) {
  file_system_.rmdir("dir1/dir3");

  auto list = file_system_.ls("dir1");
  ASSERT_EQ(list.size(), 0);
}

TEST_F(RmdirTest, RemoveNonExistingDirectory) { ASSERT_THROW(file_system_.rmdir("dir3"), std::invalid_argument); }

TEST_F(RmdirTest, RemoveRootDirectory) { ASSERT_THROW(file_system_.rmdir("/"), std::invalid_argument); }

TEST_F(RmdirTest, RemoveCurrentWorkingDirectory) {
  file_system_.cd("dir2");
  ASSERT_THROW(file_system_.rmdir("."), std::invalid_argument);
}

TEST_F(RmdirTest, RemoveDirectoryWithFile) {
  file_system_.touch("dir2/file1");
  ASSERT_THROW(file_system_.rmdir("dir2"), std::invalid_argument);
}

TEST_F(RmdirTest, RemoveDirectoryWithDirectory) { ASSERT_THROW(file_system_.rmdir("dir1"), std::invalid_argument); }