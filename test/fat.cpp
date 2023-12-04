#include "../src/FileSystem/FileSystem.hpp"
#include <gtest/gtest.h>

class FATTest : public testing::Test {
protected:
  // NOLINTBEGIN(cppcoreguidelines-non-private-member-variables-in-classes)
  std::shared_ptr<FAT> fat_;
  const std::string path = "fat_test.fs";
  const std::uint64_t size = 1024;
  const std::uint64_t cluster_size = 64;
  // NOLINTEND(cppcoreguidelines-non-private-member-variables-in-classes)

  void SetUp() override {
    FileSystem::make(path, {size, cluster_size});

    auto ifs = std::make_unique<std::ifstream>(path, std::ios::binary | std::ios::in);
    auto ofs = std::make_unique<std::ofstream>(path, std::ios::binary | std::ios::out | std::ios::in);
    if (!ifs->is_open() || !ofs->is_open()) throw std::runtime_error("Cannot open file " + path);

    auto disk_reader = std::make_shared<DiskReader>(std::move(ifs), DiskHandler::DiskOffset(0));
    auto disk_writer = std::make_shared<DiskWriter>(std::move(ofs), DiskHandler::DiskOffset(0));

    fat_ = std::make_shared<FAT>(disk_reader, disk_writer, DiskHandler::DiskOffset(FSMaker::fat_offset()),
                                 FSMaker::calculate_fat_entries_count({size, cluster_size}));
  }

  void TearDown() override { std::filesystem::remove(path); }
};

TEST_F(FATTest, EntriesCount) {
  EXPECT_EQ(fat_->entries_count(), FSMaker::calculate_fat_entries_count({size, cluster_size}));
}

TEST_F(FATTest, Allocate) {
  auto const cluster = fat_->allocate();
  EXPECT_EQ(cluster, 0);
  EXPECT_TRUE(fat_->is_allocated(cluster));
  EXPECT_TRUE(fat_->is_last(cluster));
}

TEST_F(FATTest, AllocateMultiple) {
  auto const first_cluster = fat_->allocate(5);

  auto current_cluster = first_cluster;
  for (std::uint64_t i = 0; i < 4; ++i) {
    EXPECT_TRUE(fat_->is_allocated(current_cluster));
    EXPECT_FALSE(fat_->is_last(current_cluster));
    current_cluster = fat_->get_next(current_cluster);
  }
  EXPECT_TRUE(fat_->is_allocated(current_cluster));
  EXPECT_TRUE(fat_->is_last(current_cluster));
}

TEST_F(FATTest, AllocateTooMany) { EXPECT_THROW(auto res = fat_->allocate(1000), std::runtime_error); }

TEST_F(FATTest, EmptyEntryBytes) {
  auto const empty_entry_bytes = FAT::empty_entry_bytes();
  EXPECT_EQ(empty_entry_bytes.size(), FAT::entry_size());
}