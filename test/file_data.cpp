#include "../src/FileSystem/FileData/FileData.hpp"
#include <gtest/gtest.h>

TEST(FileDataTest, Constructor) {
  const std::string name = "test";
  auto const size = FileData::FileSize{1024};
  auto const first_cluster_index = 0;
  auto const is_directory = true;

  auto const file_data = FileData{name, size, first_cluster_index, is_directory};

  EXPECT_EQ(file_data.name(), name);
  EXPECT_EQ(file_data.size().bytes, size.bytes);
  EXPECT_EQ(file_data.first_cluster_index(), first_cluster_index);
  EXPECT_EQ(file_data.is_directory(), is_directory);
}

TEST(FileDataTest, Setters) {
  const std::string name = "test";
  auto const size = FileData::FileSize{1024};
  auto const first_cluster_index = 0;
  auto const is_directory = true;

  auto file_data = FileData{name, size, first_cluster_index, is_directory};

  const std::string new_name = "new_test";
  auto const new_size = FileData::FileSize{2048};
  auto const new_first_cluster_index = 1;
  auto const new_is_directory = false;

  file_data.set_name(new_name);
  file_data.set_size(new_size);
  file_data.set_first_cluster_index(new_first_cluster_index);
  file_data.set_is_directory(new_is_directory);

  EXPECT_EQ(file_data.name(), new_name);
  EXPECT_EQ(file_data.size().bytes, new_size.bytes);
  EXPECT_EQ(file_data.first_cluster_index(), new_first_cluster_index);
  EXPECT_EQ(file_data.is_directory(), new_is_directory);
}

TEST(FileDataTest, ToBytes) {
  const std::string name = "test";
  auto const size = FileData::FileSize{1024};
  auto const first_cluster_index = 0;
  auto const is_directory = true;

  auto const file_data = FileData{name, size, first_cluster_index, is_directory};

  auto const bytes = file_data.to_bytes();

  EXPECT_EQ(bytes.size(), FileData::file_data_size());
}

TEST(FileDataTest, FromBytes) {
  const std::string name = "test";
  auto const size = FileData::FileSize{1024};
  auto const first_cluster_index = 0;
  auto const is_directory = true;

  auto const file_data = FileData{name, size, first_cluster_index, is_directory};

  auto const bytes = file_data.to_bytes();

  auto const new_file_data = FileData::from_bytes(bytes);

  EXPECT_EQ(new_file_data.name(), name);
  EXPECT_EQ(new_file_data.size().bytes, size.bytes);
  EXPECT_EQ(new_file_data.first_cluster_index(), first_cluster_index);
  EXPECT_EQ(new_file_data.is_directory(), is_directory);
}