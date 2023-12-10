#include "../src/FileSystem/FileSystem.hpp"
#include <filesystem>
#include <gtest/gtest.h>

class DirnameBasenameTest : public testing::Test {
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

TEST_F(DirnameBasenameTest, Dirname) {
  EXPECT_EQ(file_system_.dirname("/"), "/");
  EXPECT_EQ(file_system_.dirname("/a"), "/");
  EXPECT_EQ(file_system_.dirname("/a/"), "/");
  EXPECT_EQ(file_system_.dirname("/a/b"), "/a");
  EXPECT_EQ(file_system_.dirname("/a/b/"), "/a");
  EXPECT_EQ(file_system_.dirname("/a/b/c"), "/a/b");
  EXPECT_EQ(file_system_.dirname("/a/b/c/"), "/a/b");

  EXPECT_EQ(file_system_.dirname("a"), ".");
}

TEST_F(DirnameBasenameTest, Basename) {
  EXPECT_EQ(file_system_.basename("/"), "/");
  EXPECT_EQ(file_system_.basename("/a"), "a");
  EXPECT_EQ(file_system_.basename("/a/"), "a");
  EXPECT_EQ(file_system_.basename("/a/b"), "b");
  EXPECT_EQ(file_system_.basename("/a/b/"), "b");
  EXPECT_EQ(file_system_.basename("/a/b/c"), "c");
  EXPECT_EQ(file_system_.basename("/a/b/c/"), "c");

  EXPECT_EQ(file_system_.basename("a"), "a");
}

TEST_F(DirnameBasenameTest, DirnameMeaningfulFilenames) {
  EXPECT_EQ(file_system_.dirname("/user"), "/");
  EXPECT_EQ(file_system_.dirname("/user/"), "/");
  EXPECT_EQ(file_system_.dirname("/user/desktop"), "/user");
  EXPECT_EQ(file_system_.dirname("/user/desktop/"), "/user");
  EXPECT_EQ(file_system_.dirname("/user/desktop/file.txt"), "/user/desktop");
  EXPECT_EQ(file_system_.dirname("/user/desktop/file.txt/"), "/user/desktop");
}

TEST_F(DirnameBasenameTest, BasenameMeaningfulFilenames) {
  EXPECT_EQ(file_system_.basename("/user"), "user");
  EXPECT_EQ(file_system_.basename("/user/"), "user");
  EXPECT_EQ(file_system_.basename("/user/desktop"), "desktop");
  EXPECT_EQ(file_system_.basename("/user/desktop/"), "desktop");
  EXPECT_EQ(file_system_.basename("/user/desktop/file.txt"), "file.txt");
  EXPECT_EQ(file_system_.basename("/user/desktop/file.txt/"), "file.txt");
}

TEST_F(DirnameBasenameTest, DirnameEmpty) { EXPECT_THROW(auto res = file_system_.dirname(""), std::invalid_argument); }

TEST_F(DirnameBasenameTest, BasenameEmpty) {
  EXPECT_THROW(auto res = file_system_.basename(""), std::invalid_argument);
}

TEST_F(DirnameBasenameTest, DirnameRoot) { EXPECT_EQ(file_system_.dirname("/"), "/"); }

TEST_F(DirnameBasenameTest, BasenameRoot) { EXPECT_EQ(file_system_.basename("/"), "/"); }

TEST_F(DirnameBasenameTest, DirnameLong) {
  std::string const path = "/a/b/c/d/e/f/g/h/i/j/k/l/m/n/o/p/q/r/s/t/u/v/w/x/y/z";

  EXPECT_EQ(file_system_.dirname(path), "/a/b/c/d/e/f/g/h/i/j/k/l/m/n/o/p/q/r/s/t/u/v/w/x/y");
}

TEST_F(DirnameBasenameTest, BasenameLong) {
  std::string const path = "/a/b/c/d/e/f/g/h/i/j/k/l/m/n/o/p/q/r/s/t/u/v/w/x/y/z";

  EXPECT_EQ(file_system_.basename(path), "z");
}