#include "../src/FileSystem/FileSystem.hpp"
#include <gtest/gtest.h>

class MkdirTest : public testing::Test {
protected:
  // NOLINTBEGIN(cppcoreguidelines-non-private-member-variables-in-classes)
  std::string const PATH = "test.fs";
  std::uint64_t const SIZE = 2024;
  std::uint64_t const CLUSTER_SIZE = 64;

  std::unique_ptr<FileSystem> file_system_;
  // NOLINTEND(cppcoreguidelines-non-private-member-variables-in-classes)

  void SetUp() override {
    FileSystem::make(PATH, {SIZE, CLUSTER_SIZE});

    file_system_ = std::make_unique<FileSystem>(PATH);
  }

  void TearDown() override { std::filesystem::remove(PATH); }
};

TEST_F(MkdirTest, Empty) {
  file_system_->mkdir("./a");
  file_system_->mkdir("b");
  file_system_->mkdir("/c");
  auto const list = file_system_->ls("/");
  EXPECT_EQ(list.size(), 5);
  EXPECT_EQ(list[0].name(), ".");
  EXPECT_EQ(list[1].name(), "..");
  EXPECT_EQ(list[2].name(), "a");
  EXPECT_EQ(list[3].name(), "b");
  EXPECT_EQ(list[4].name(), "c");
}

TEST_F(MkdirTest, NonExistent) { EXPECT_THROW(file_system_->mkdir("/non_existent/test"), std::invalid_argument); }

TEST_F(MkdirTest, NonExistentNested) {
  EXPECT_THROW(file_system_->mkdir("/non_existent/nested/test"), std::invalid_argument);
}

TEST_F(MkdirTest, AlreadyExists) {
  file_system_->mkdir("/test");
  EXPECT_THROW(file_system_->mkdir("/test"), std::invalid_argument);
}