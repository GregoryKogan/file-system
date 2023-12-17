#include "../src/FileSystem/FileSystem.hpp"
#include <filesystem>
#include <gtest/gtest.h>

class FATTest : public testing::Test {
protected:
  // NOLINTBEGIN(cppcoreguidelines-non-private-member-variables-in-classes)
  FAT fat_;
  const std::string PATH = "fat_test.fs";
  const std::uint64_t SIZE = 1024;
  const std::uint64_t CLUSTER_SIZE = 64;
  // NOLINTEND(cppcoreguidelines-non-private-member-variables-in-classes)

  auto SetUp() -> void override {
    FileSystem::make(PATH, {SIZE, CLUSTER_SIZE});

    auto ifs = std::make_unique<std::ifstream>(PATH, std::ios::binary | std::ios::in);
    auto ofs = std::make_unique<std::ofstream>(PATH, std::ios::binary | std::ios::out | std::ios::in);
    if (!ifs->is_open() || !ofs->is_open()) throw std::runtime_error("Cannot open file " + PATH);

    auto disk_reader = DiskReader(std::move(ifs), 0, 0);
    auto disk_writer = DiskWriter(std::move(ofs), 0);

    fat_ = FAT(disk_reader, disk_writer, FSMaker::get_fat_offset(),
               FSMaker::calculate_fat_entries_count({SIZE, CLUSTER_SIZE}));
  }

  auto TearDown() -> void override { std::filesystem::remove(PATH); }
};

TEST_F(FATTest, EntriesCount) {
  EXPECT_EQ(fat_.get_clusters_count(), FSMaker::calculate_fat_entries_count({SIZE, CLUSTER_SIZE}));
}

TEST_F(FATTest, Allocate) {
  auto const cluster = fat_.allocate();
  EXPECT_EQ(cluster, 0);
  EXPECT_TRUE(fat_.is_allocated(cluster));
  EXPECT_TRUE(fat_.is_last(cluster));
}

TEST_F(FATTest, AllocateMultiple) {
  auto const first_cluster = fat_.allocate();
  auto current_cluster = first_cluster;
  for (int i = 0; i < 4; ++i) current_cluster = fat_.allocate_next(current_cluster);

  current_cluster = first_cluster;
  for (std::uint64_t i = 0; i < 4; ++i) {
    EXPECT_TRUE(fat_.is_allocated(current_cluster));
    EXPECT_FALSE(fat_.is_last(current_cluster));
    current_cluster = fat_.get_next(current_cluster);
  }
  EXPECT_TRUE(fat_.is_allocated(current_cluster));
  EXPECT_TRUE(fat_.is_last(current_cluster));
}

TEST_F(FATTest, AllocateTooMany) {
  EXPECT_THROW(
      [this]() {
        for (int i = 0; i < 100; ++i) { auto tmp = fat_.allocate(); }
      }(),
      std::runtime_error);
}

TEST_F(FATTest, EmptyEntryBytes) {
  auto const empty_entry_bytes = FAT::get_empty_entry_bytes();
  EXPECT_EQ(empty_entry_bytes.size(), FAT::get_entry_size());
}