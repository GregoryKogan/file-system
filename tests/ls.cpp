#include "../src/FileSystem/FileSystem.hpp"
#include <filesystem>
#include <gtest/gtest.h>

class LsTest : public testing::Test {
protected:
  // NOLINTBEGIN(cppcoreguidelines-non-private-member-variables-in-classes)
  std::string const PATH = "test.fs";
  std::uint64_t const SIZE = 1024;
  std::uint64_t const CLUSTER_SIZE = 64;

  FileSystem file_system_;
  // NOLINTEND(cppcoreguidelines-non-private-member-variables-in-classes)

  auto SetUp() -> void override {
    FileSystem::make(PATH, {SIZE, CLUSTER_SIZE});
    file_system_ = FileSystem(PATH);
  }

  auto TearDown() -> void override { std::filesystem::remove(PATH); }
};

TEST_F(LsTest, Empty) {
  auto const list = file_system_.ls("/");
  EXPECT_EQ(list.size(), 0);
}

TEST_F(LsTest, NonExistent) { EXPECT_THROW(auto const list = file_system_.ls("/non_existent"), std::invalid_argument); }

TEST_F(LsTest, NonExistentNested) {
  EXPECT_THROW(auto const list = file_system_.ls("/non_existent/nested"), std::invalid_argument);
}

TEST_F(LsTest, MultipleFiles) {
  file_system_.mkdir("/dir1");
  file_system_.mkdir("/dir2");
  file_system_.mkdir("/dir3");

  auto const list = file_system_.ls("/");
  EXPECT_EQ(list.size(), 3);
  EXPECT_EQ(list[0].get_name(), "dir1");
  EXPECT_EQ(list[1].get_name(), "dir2");
  EXPECT_EQ(list[2].get_name(), "dir3");
}

TEST_F(LsTest, MultipleFilesNested) {
  file_system_.mkdir("/dir1");
  file_system_.mkdir("/dir2");
  file_system_.mkdir("/dir1/dir3");

  auto const list = file_system_.ls("/dir1");
  EXPECT_EQ(list.size(), 1);
  EXPECT_EQ(list[0].get_name(), "dir3");
}

TEST_F(LsTest, MultipleFilesNestedEmpty) {
  file_system_.mkdir("/dir1");
  file_system_.mkdir("/dir2");
  file_system_.mkdir("/dir1/dir3");

  auto const list = file_system_.ls("/dir1/dir3");
  EXPECT_EQ(list.size(), 0);
}