#include "../src/FileSystem/Directory/Directory.hpp"
#include <gtest/gtest.h>

TEST(DirectoryTest, MakeRoot) {
  auto const root_dir = Directory::make_root();

  EXPECT_EQ(root_dir.files().size(), 2);
  EXPECT_EQ(root_dir.files()[0].name(), ".");
  EXPECT_EQ(root_dir.files()[0].size().bytes, FileData::file_data_size() * 2);
  EXPECT_EQ(root_dir.files()[0].first_cluster_index(), 0);
  EXPECT_EQ(root_dir.files()[0].is_directory(), true);
  EXPECT_EQ(root_dir.files()[1].name(), "..");
  EXPECT_EQ(root_dir.files()[1].size().bytes, 0);
  EXPECT_EQ(root_dir.files()[1].first_cluster_index(), 0);
  EXPECT_EQ(root_dir.files()[1].is_directory(), true);
}

TEST(DirectoryTest, MakeEmpty) {
  auto const parent_dir = FileData("..", FileData::FileSize{FileData::file_data_size() * 2}, 0, true);
  auto const empty_dir = Directory::make_empty(parent_dir, 0);

  EXPECT_EQ(empty_dir.files().size(), 2);
  EXPECT_EQ(empty_dir.files()[0].name(), ".");
  EXPECT_EQ(empty_dir.files()[0].size().bytes, FileData::file_data_size() * 2);
  EXPECT_EQ(empty_dir.files()[0].first_cluster_index(), 0);
  EXPECT_EQ(empty_dir.files()[0].is_directory(), true);
  EXPECT_EQ(empty_dir.files()[1].name(), "..");
  EXPECT_EQ(empty_dir.files()[1].size().bytes, 0);
  EXPECT_EQ(empty_dir.files()[1].first_cluster_index(), 0);
  EXPECT_EQ(empty_dir.files()[1].is_directory(), true);
}

TEST(DirectoryTest, ByteConversion) {
  auto const parent_dir = FileData("..", FileData::FileSize{FileData::file_data_size() * 2}, 0, true);
  auto const empty_dir = Directory::make_empty(parent_dir, 0);

  auto const bytes = empty_dir.to_bytes();
  auto const actual = Directory::from_bytes(bytes);

  EXPECT_EQ(actual.files().size(), empty_dir.files().size());
  EXPECT_EQ(actual.files().size(), 2);
  EXPECT_EQ(actual.files()[0].name(), empty_dir.files()[0].name());
  EXPECT_EQ(actual.files()[0].size().bytes, empty_dir.files()[0].size().bytes);
  EXPECT_EQ(actual.files()[0].first_cluster_index(), empty_dir.files()[0].first_cluster_index());
  EXPECT_EQ(actual.files()[0].is_directory(), empty_dir.files()[0].is_directory());
  EXPECT_EQ(actual.files()[1].name(), empty_dir.files()[1].name());
  EXPECT_EQ(actual.files()[1].size().bytes, empty_dir.files()[1].size().bytes);
  EXPECT_EQ(actual.files()[1].first_cluster_index(), empty_dir.files()[1].first_cluster_index());
  EXPECT_EQ(actual.files()[1].is_directory(), empty_dir.files()[1].is_directory());
}

TEST(DirectoryTest, AddFile) {
  auto dir = Directory::make_root();

  auto const file = FileData("file.txt", FileData::FileSize{64}, 12, false);
  dir.add_file(file);

  EXPECT_EQ(dir.files().size(), 3);
  EXPECT_EQ(dir.files()[0].size().bytes, FileData::file_data_size() * 3);

  EXPECT_EQ(dir.files()[2].name(), "file.txt");
  EXPECT_EQ(dir.files()[2].size().bytes, 64);
  EXPECT_EQ(dir.files()[2].first_cluster_index(), 12);
  EXPECT_EQ(dir.files()[2].is_directory(), false);
}

TEST(DirectoryTest, AddFileAlreadyExists) {
  auto dir = Directory::make_root();

  auto const file = FileData("file.txt", FileData::FileSize{64}, 12, false);
  dir.add_file(file);

  EXPECT_THROW(dir.add_file(file), std::runtime_error);
}

TEST(DirectoryTest, Find) {
  auto dir = Directory::make_root();

  auto const file = FileData("file.txt", FileData::FileSize{64}, 12, false);
  dir.add_file(file);

  auto const actual = dir.find("file.txt");

  // NOLINTBEGIN(bugprone-unchecked-optional-access)
  ASSERT_TRUE(actual.has_value());
  EXPECT_EQ(actual.value().name(), "file.txt");
  EXPECT_EQ(actual.value().size().bytes, 64);
  EXPECT_EQ(actual.value().first_cluster_index(), 12);
  EXPECT_EQ(actual.value().is_directory(), false);
  // NOLINTEND(bugprone-unchecked-optional-access)
}

TEST(DirectoryTest, FindNotFound) {
  auto dir = Directory::make_root();

  auto const file = FileData("file.txt", FileData::FileSize{64}, 12, false);
  dir.add_file(file);

  auto const actual = dir.find("file2.txt");

  EXPECT_FALSE(actual.has_value());
}

TEST(DirectoryTest, FindEmpty) {
  auto dir = Directory::make_root();

  auto const actual = dir.find("file.txt");

  EXPECT_FALSE(actual.has_value());
}

TEST(DirectoryTest, FindEmptyName) {
  auto dir = Directory::make_root();

  auto const actual = dir.find("");

  EXPECT_FALSE(actual.has_value());
}