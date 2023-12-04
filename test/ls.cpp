#include "../src/FileSystem/FileSystem.hpp"
#include <filesystem>
#include <gtest/gtest.h>

class LsTest : public testing::Test {
protected:
  // NOLINTBEGIN(cppcoreguidelines-non-private-member-variables-in-classes)
  std::string const PATH = "test.fs";
  std::uint64_t const SIZE = 1024;
  std::uint64_t const CLUSTER_SIZE = 64;

  std::unique_ptr<FileSystem> file_system_;
  // NOLINTEND(cppcoreguidelines-non-private-member-variables-in-classes)

  void SetUp() override {
    FileSystem::make(PATH, {SIZE, CLUSTER_SIZE});

    file_system_ = std::make_unique<FileSystem>(PATH);
  }

  void TearDown() override { std::filesystem::remove(PATH); }
};

TEST_F(LsTest, Empty) {
  auto const list = file_system_->ls("/");
  EXPECT_EQ(list.size(), 2);
  EXPECT_EQ(list[0].name(), ".");
  EXPECT_EQ(list[1].name(), "..");
}

TEST_F(LsTest, NonExistent) {
  EXPECT_THROW(auto const list = file_system_->ls("/non_existent"), std::invalid_argument);
}

TEST_F(LsTest, NonExistentNested) {
  EXPECT_THROW(auto const list = file_system_->ls("/non_existent/nested"), std::invalid_argument);
}