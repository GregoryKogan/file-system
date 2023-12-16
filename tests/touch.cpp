#include "../src/FileSystem/FileSystem.hpp"
#include <filesystem>
#include <gtest/gtest.h>

class TouchTest : public testing::Test {
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

TEST_F(TouchTest, TouchFile) {
  file_system_.touch("file1");
  auto list = file_system_.ls("/");
  ASSERT_EQ(list.size(), 3);
  EXPECT_EQ(list[0].get_name(), "dir1");
  EXPECT_EQ(list[1].get_name(), "dir2");
  EXPECT_EQ(list[2].get_name(), "file1");

  EXPECT_EQ(list[2].get_size(), 0);
  EXPECT_EQ(list[2].get_parent_first_cluster(), 0);
}

TEST_F(TouchTest, TouchFileInDir) {
  file_system_.touch("dir1/file1");
  auto list = file_system_.ls("/dir1");
  ASSERT_EQ(list.size(), 2);
  EXPECT_EQ(list[0].get_name(), "dir3");
  EXPECT_EQ(list[1].get_name(), "file1");
  EXPECT_EQ(list[1].get_size(), 0);
  EXPECT_NE(list[1].get_parent_first_cluster(), 0);
}

TEST_F(TouchTest, TouchFileInNestedDir) {
  file_system_.touch("dir1/dir3/file1");
  auto list = file_system_.ls("/dir1/dir3");
  ASSERT_EQ(list.size(), 1);
  EXPECT_EQ(list[0].get_name(), "file1");
  EXPECT_EQ(list[0].get_size(), 0);
  EXPECT_NE(list[0].get_parent_first_cluster(), 0);
}

TEST_F(TouchTest, TouchMultipleFilesInNestedDir) {
  file_system_.touch("dir1/dir3/file1");
  file_system_.touch("dir1/dir3/file2");
  auto list = file_system_.ls("/dir1/dir3");
  ASSERT_EQ(list.size(), 2);
  EXPECT_EQ(list[0].get_name(), "file1");
  EXPECT_EQ(list[1].get_name(), "file2");
  EXPECT_EQ(list[0].get_size(), 0);
  EXPECT_EQ(list[1].get_size(), 0);
  EXPECT_NE(list[0].get_parent_first_cluster(), 0);
  EXPECT_NE(list[1].get_parent_first_cluster(), 0);
}

TEST_F(TouchTest, TouchExistingFile) {
  file_system_.touch("dir1/dir3/file1");
  file_system_.touch("dir1/dir3/file1");
  auto list = file_system_.ls("/dir1/dir3");
  ASSERT_EQ(list.size(), 1);
  EXPECT_EQ(list[0].get_name(), "file1");
  EXPECT_EQ(list[0].get_size(), 0);
  EXPECT_NE(list[0].get_parent_first_cluster(), 0);
}

TEST_F(TouchTest, TouchFileInNonExistingDir) {
  EXPECT_THROW(file_system_.touch("dir1/dir4/file1"), std::invalid_argument);
}

TEST_F(TouchTest, TouchFileInFile) {
  file_system_.touch("file1");
  EXPECT_THROW(file_system_.touch("file1/file2"), std::invalid_argument);
}

TEST_F(TouchTest, TouchFileWithInvalidName) {
  EXPECT_THROW(file_system_.touch("file1//file2"), std::invalid_argument);
  EXPECT_THROW(file_system_.touch("file1/."), std::invalid_argument);
  EXPECT_THROW(file_system_.touch("file1/.."), std::invalid_argument);
  EXPECT_THROW(file_system_.touch("file1/./file2"), std::invalid_argument);
  EXPECT_THROW(file_system_.touch("file1/../file2"), std::invalid_argument);
  EXPECT_THROW(file_system_.touch("file1/./."), std::invalid_argument);
  EXPECT_THROW(file_system_.touch("file1/../.."), std::invalid_argument);
  EXPECT_THROW(file_system_.touch("file1/./.."), std::invalid_argument);
  EXPECT_THROW(file_system_.touch("file1/../."), std::invalid_argument);
  EXPECT_THROW(file_system_.touch("file1/././file2"), std::invalid_argument);
  EXPECT_THROW(file_system_.touch("file1/../../file2"), std::invalid_argument);
  EXPECT_THROW(file_system_.touch("file1/././."), std::invalid_argument);
  EXPECT_THROW(file_system_.touch("file1/../../.."), std::invalid_argument);
  EXPECT_THROW(file_system_.touch("file1/./../.."), std::invalid_argument);
  EXPECT_THROW(file_system_.touch("file1/.././.."), std::invalid_argument);
  EXPECT_THROW(file_system_.touch("file1/./../."), std::invalid_argument);
  EXPECT_THROW(file_system_.touch("file1/./././file2"), std::invalid_argument);
  EXPECT_THROW(file_system_.touch("file1/../../../file2"), std::invalid_argument);
  EXPECT_THROW(file_system_.touch("file1/./././."), std::invalid_argument);
  EXPECT_THROW(file_system_.touch("file1/../../../../.."), std::invalid_argument);
  EXPECT_THROW(file_system_.touch("file1/./././../.."), std::invalid_argument);
  EXPECT_THROW(file_system_.touch("file1/././.././.."), std::invalid_argument);
  EXPECT_THROW(file_system_.touch("file1/./../././.."), std::invalid_argument);
  EXPECT_THROW(file_system_.touch("file1/.././././.."), std::invalid_argument);
}