#include "../src/FileSystem/FileSystem.hpp"
#include <filesystem>
#include <gtest/gtest.h>

class RmTest : public testing::Test {
protected:
  // NOLINTBEGIN(cppcoreguidelines-non-private-member-variables-in-classes)
  std::string const PATH = "test.fs";
  std::uint64_t const SIZE = 32000;
  std::uint64_t const CLUSTER_SIZE = 256;

  FileSystem file_system_;
  // NOLINTEND(cppcoreguidelines-non-private-member-variables-in-classes)

  auto SetUp() -> void override {
    FileSystem::make(PATH, {SIZE, CLUSTER_SIZE});
    file_system_ = FileSystem(PATH);
    file_system_.mkdir("empty");
    file_system_.mkdir("dir");
    file_system_.mkdir("dir/nested");
    file_system_.mkdir("dir/nested/empty");
    file_system_.touch("file");
    file_system_.touch("dir/file");
    file_system_.touch("dir/nested/file");
  }

  auto TearDown() -> void override { std::filesystem::remove(PATH); }
};

TEST_F(RmTest, RemoveFile) {
  file_system_.rm("file");
  file_system_.rm("dir/file");
  file_system_.rm("dir/nested/file");

  auto root_list = file_system_.ls("/");
  ASSERT_EQ(root_list.size(), 2);
  EXPECT_EQ(root_list[0].get_name(), "empty");
  EXPECT_EQ(root_list[1].get_name(), "dir");

  auto dir_list = file_system_.ls("dir");
  ASSERT_EQ(dir_list.size(), 1);
  EXPECT_EQ(dir_list[0].get_name(), "nested");

  auto nested_list = file_system_.ls("dir/nested");
  ASSERT_EQ(nested_list.size(), 1);
  EXPECT_EQ(nested_list[0].get_name(), "empty");
}

TEST_F(RmTest, RemoveEmptyDirectory) {
  file_system_.rm("empty");
  file_system_.rm("dir/nested/empty");

  auto root_list = file_system_.ls("/");
  ASSERT_EQ(root_list.size(), 2);
  EXPECT_EQ(root_list[0].get_name(), "dir");
  EXPECT_EQ(root_list[1].get_name(), "file");

  auto nested_list = file_system_.ls("dir/nested");
  ASSERT_EQ(nested_list.size(), 1);
  EXPECT_EQ(nested_list[0].get_name(), "file");
}

TEST_F(RmTest, RemoveRecursivelyEmptyDirectory) {
  file_system_.rm("empty", true);
  file_system_.rm("dir/nested/empty", true);

  auto root_list = file_system_.ls("/");
  ASSERT_EQ(root_list.size(), 2);
  EXPECT_EQ(root_list[0].get_name(), "dir");
  EXPECT_EQ(root_list[1].get_name(), "file");

  auto nested_list = file_system_.ls("dir/nested");
  ASSERT_EQ(nested_list.size(), 1);
  EXPECT_EQ(nested_list[0].get_name(), "file");
}

TEST_F(RmTest, RemoveNonEmptyDirectory) {
  EXPECT_THROW(file_system_.rm("dir"), std::invalid_argument);
  EXPECT_THROW(file_system_.rm("dir/nested"), std::invalid_argument);
}

TEST_F(RmTest, RemoveRecursivelyNonEmptyDirectory) {
  file_system_.rm("dir", true);

  auto root_list = file_system_.ls("/");
  ASSERT_EQ(root_list.size(), 2);
  EXPECT_EQ(root_list[0].get_name(), "empty");
  EXPECT_EQ(root_list[1].get_name(), "file");
}

TEST_F(RmTest, RemoveNonExisting) {
  EXPECT_THROW(file_system_.rm("non_existing"), std::invalid_argument);
  EXPECT_THROW(file_system_.rm("dir/non_existing"), std::invalid_argument);
  EXPECT_THROW(file_system_.rm("dir/nested/non_existing"), std::invalid_argument);
}

TEST_F(RmTest, RemoveRecursivelyNonExisting) {
  EXPECT_THROW(file_system_.rm("non_existing", true), std::invalid_argument);
  EXPECT_THROW(file_system_.rm("dir/non_existing", true), std::invalid_argument);
  EXPECT_THROW(file_system_.rm("dir/nested/non_existing", true), std::invalid_argument);
}

TEST_F(RmTest, RemoveRoot) { EXPECT_THROW(file_system_.rm("/"), std::invalid_argument); }

TEST_F(RmTest, RemoveRecursivelyRoot) { EXPECT_THROW(file_system_.rm("/", true), std::invalid_argument); }

TEST_F(RmTest, RemoveCurrentWorkingDirectory) {
  file_system_.cd("empty");
  EXPECT_THROW(file_system_.rm("."), std::invalid_argument);
  EXPECT_THROW(file_system_.rm("/empty"), std::invalid_argument);
}

TEST_F(RmTest, RemoveRecursivelyCurrentWorkingDirectory) {
  file_system_.cd("empty");
  EXPECT_THROW(file_system_.rm(".", true), std::invalid_argument);
  EXPECT_THROW(file_system_.rm("/empty", true), std::invalid_argument);
}

TEST_F(RmTest, RemoveParentDirectory) {
  file_system_.cd("dir/nested");
  EXPECT_THROW(file_system_.rm(".."), std::invalid_argument);
  EXPECT_THROW(file_system_.rm("../.."), std::invalid_argument);
  EXPECT_THROW(file_system_.rm("/dir"), std::invalid_argument);

  auto root_list = file_system_.ls("/");
  ASSERT_EQ(root_list.size(), 3);
  EXPECT_EQ(root_list[0].get_name(), "empty");
  EXPECT_EQ(root_list[1].get_name(), "dir");
  EXPECT_EQ(root_list[2].get_name(), "file");

  auto dir_list = file_system_.ls("/dir");
  ASSERT_EQ(dir_list.size(), 2);
  EXPECT_EQ(dir_list[0].get_name(), "nested");
  EXPECT_EQ(dir_list[1].get_name(), "file");

  auto nested_list = file_system_.ls("/dir/nested");
  ASSERT_EQ(nested_list.size(), 2);
  EXPECT_EQ(nested_list[0].get_name(), "empty");
  EXPECT_EQ(nested_list[1].get_name(), "file");
}

TEST_F(RmTest, RemoveRecursivelyParentDirectory) {
  file_system_.cd("dir/nested");
  EXPECT_THROW(file_system_.rm("..", true), std::invalid_argument);
  EXPECT_THROW(file_system_.rm("../..", true), std::invalid_argument);
  EXPECT_THROW(file_system_.rm("/dir", true), std::invalid_argument);

  auto root_list = file_system_.ls("/");
  ASSERT_EQ(root_list.size(), 3);
  EXPECT_EQ(root_list[0].get_name(), "empty");
  EXPECT_EQ(root_list[1].get_name(), "dir");
  EXPECT_EQ(root_list[2].get_name(), "file");

  auto dir_list = file_system_.ls("/dir");
  ASSERT_EQ(dir_list.size(), 2);
  EXPECT_EQ(dir_list[0].get_name(), "nested");
  EXPECT_EQ(dir_list[1].get_name(), "file");

  auto nested_list = file_system_.ls("/dir/nested");
  ASSERT_EQ(nested_list.size(), 2);
  EXPECT_EQ(nested_list[0].get_name(), "empty");
  EXPECT_EQ(nested_list[1].get_name(), "file");
}