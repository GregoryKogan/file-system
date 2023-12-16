#include "../src/FileSystem/FileSystem.hpp"
#include <filesystem>
#include <gtest/gtest.h>

class CdTest : public testing::Test {
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

TEST_F(CdTest, Empty) {
  file_system_.mkdir("/dir1");
  file_system_.mkdir("/dir2");
  file_system_.mkdir("/dir3");

  file_system_.cd("/dir1");
  EXPECT_EQ(file_system_.pwd(), "/dir1");
  file_system_.cd("/dir2");
  EXPECT_EQ(file_system_.pwd(), "/dir2");
  file_system_.cd("/dir3");
  EXPECT_EQ(file_system_.pwd(), "/dir3");
}

TEST_F(CdTest, NonExistent) { EXPECT_THROW(file_system_.cd("/non_existent"), std::invalid_argument); }

TEST_F(CdTest, NonExistentNested) {
  file_system_.mkdir("/dir1");
  file_system_.mkdir("/dir2");
  file_system_.mkdir("/dir1/dir3");

  EXPECT_THROW(file_system_.cd("/dir1/non_existent"), std::invalid_argument);
}

TEST_F(CdTest, MultipleFilesNested) {
  file_system_.mkdir("dir1");
  file_system_.mkdir("dir2");
  file_system_.mkdir("dir1/dir3");

  file_system_.cd("dir1");
  EXPECT_EQ(file_system_.pwd(), "/dir1");
  file_system_.cd("dir3");
  EXPECT_EQ(file_system_.pwd(), "/dir1/dir3");
  file_system_.cd("/dir2");
  EXPECT_EQ(file_system_.pwd(), "/dir2");
}