#include "../src/FileSystem/FileSystem.hpp"
#include <filesystem>
#include <gtest/gtest.h>

class CpTest : public testing::Test {
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

TEST_F(CpTest, CopyShortFile) {
  file_system_.mkdir("dir");
  file_system_.cp("samples/short.txt", "dir/short_copy.txt");

  std::ostringstream original;
  file_system_.cat("samples/short.txt", original);

  std::ostringstream copy;
  file_system_.cat("dir/short_copy.txt", copy);

  EXPECT_EQ(original.str(), copy.str());
}

TEST_F(CpTest, CopyLongFile) {
  file_system_.mkdir("dir");
  file_system_.cp("samples/long.txt", "dir/long_copy.txt");

  std::ostringstream original;
  file_system_.cat("samples/long.txt", original);

  std::ostringstream copy;
  file_system_.cat("dir/long_copy.txt", copy);

  EXPECT_EQ(original.str(), copy.str());
}

TEST_F(CpTest, CopyFileRecursively) {
  file_system_.mkdir("dir");
  file_system_.cp("samples/long.txt", "dir/long_copy.txt", true);

  std::ostringstream original;
  file_system_.cat("samples/long.txt", original);

  std::ostringstream copy;
  file_system_.cat("dir/long_copy.txt", copy);

  EXPECT_EQ(original.str(), copy.str());
}

TEST_F(CpTest, CopyNonExistingFile) {
  EXPECT_THROW(file_system_.cp("samples/non_existing.txt", "dir/non_existing_copy.txt"), std::invalid_argument);
}

TEST_F(CpTest, CopyNonExistingDirectory) {
  EXPECT_THROW(file_system_.cp("non_existing", "copy", true), std::invalid_argument);
}

TEST_F(CpTest, CopyFileToNonExistingDirectory) {
  EXPECT_THROW(file_system_.cp("samples/short.txt", "non_existing_dir/short_copy.txt"), std::invalid_argument);
}

TEST_F(CpTest, DestinationDirectoryAlreadyExists) {
  file_system_.mkdir("dir");
  EXPECT_THROW(file_system_.cp("samples", "dir", true), std::invalid_argument);
}

TEST_F(CpTest, DestinationFileAlreadyExists) {
  file_system_.mkdir("dir");
  file_system_.cp("samples/short.txt", "dir/short_copy.txt");
  EXPECT_THROW(file_system_.cp("samples/short.txt", "dir/short_copy.txt"), std::invalid_argument);
}

TEST_F(CpTest, CopyDirectory) {
  file_system_.mkdir("original_dir");
  file_system_.mkdir("original_dir/nested");
  file_system_.cp("samples/short.txt", "original_dir/short.txt");
  file_system_.cp("samples/long.txt", "original_dir/nested/long.txt");

  file_system_.cp("original_dir", "copy_dir", true);

  std::ostringstream original_short;
  std::ostringstream original_long;
  file_system_.cat("original_dir/short.txt", original_short);
  file_system_.cat("original_dir/nested/long.txt", original_long);

  std::ostringstream copy_short;
  std::ostringstream copy_long;
  file_system_.cat("copy_dir/short.txt", copy_short);
  file_system_.cat("copy_dir/nested/long.txt", copy_long);

  EXPECT_EQ(original_short.str(), copy_short.str());
  EXPECT_EQ(original_long.str(), copy_long.str());
}

TEST_F(CpTest, CopyNonEmptyDirectoryNonRecursively) {
  file_system_.mkdir("original_dir");
  file_system_.mkdir("original_dir/nested");
  file_system_.cp("samples/short.txt", "original_dir/short.txt");
  file_system_.cp("samples/long.txt", "original_dir/nested/long.txt");

  EXPECT_THROW(file_system_.cp("original_dir", "copy_dir"), std::invalid_argument);
}

TEST_F(CpTest, CopyEmptyDirectory) {
  file_system_.mkdir("original_dir");

  file_system_.cp("original_dir", "copy_dir");

  auto list = file_system_.ls(".");
  ASSERT_EQ(list.size(), 3);
  EXPECT_EQ(list[0].get_name(), "samples");
  EXPECT_EQ(list[1].get_name(), "original_dir");
  EXPECT_EQ(list[2].get_name(), "copy_dir");
}

TEST_F(CpTest, CopyEmptyDirectoryRecursively) {
  file_system_.mkdir("original_dir");

  file_system_.cp("original_dir", "copy_dir", true);

  auto list = file_system_.ls(".");
  ASSERT_EQ(list.size(), 3);
  EXPECT_EQ(list[0].get_name(), "samples");
  EXPECT_EQ(list[1].get_name(), "original_dir");
  EXPECT_EQ(list[2].get_name(), "copy_dir");
}
