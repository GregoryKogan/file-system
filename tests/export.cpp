#include "../src/FileSystem/FileSystem.hpp"
#include <filesystem>
#include <gtest/gtest.h>

class ExportTest : public testing::Test {
protected:
  // NOLINTBEGIN(cppcoreguidelines-non-private-member-variables-in-classes)
  std::string const PATH = "test.fs";
  std::uint64_t const SIZE = 32768;
  std::uint64_t const CLUSTER_SIZE = 64;

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

TEST_F(ExportTest, ExportShortFile) {
  std::ofstream export_stream("exported_short.txt");
  if (!export_stream.is_open()) throw std::runtime_error("Cannot open file exported_short.txt");
  file_system_.export_file("samples/short.txt", export_stream);
  export_stream.close();

  std::ifstream exported_file("exported_short.txt");
  if (!exported_file.is_open()) throw std::runtime_error("Cannot open file exported_short.txt");
  std::string const exported_file_content((std::istreambuf_iterator<char>(exported_file)),
                                          std::istreambuf_iterator<char>());
  exported_file.close();

  std::ostringstream oss;
  file_system_.cat("samples/short.txt", oss);

  EXPECT_EQ(exported_file_content, oss.str());
}

TEST_F(ExportTest, ExportLongFile) {
  std::ofstream export_stream("exported_long.txt");
  if (!export_stream.is_open()) throw std::runtime_error("Cannot open file exported_long.txt");
  file_system_.export_file("samples/long.txt", export_stream);
  export_stream.close();

  std::ifstream exported_file("exported_long.txt");
  if (!exported_file.is_open()) throw std::runtime_error("Cannot open file exported_long.txt");
  std::string const exported_file_content((std::istreambuf_iterator<char>(exported_file)),
                                          std::istreambuf_iterator<char>());
  exported_file.close();

  std::ostringstream oss;
  file_system_.cat("samples/long.txt", oss);

  EXPECT_EQ(exported_file_content, oss.str());
}

TEST_F(ExportTest, ExportNonExistingFile) {
  std::ofstream export_stream("exported.txt");
  if (!export_stream.is_open()) throw std::runtime_error("Cannot open file exported.txt");
  EXPECT_THROW(file_system_.export_file("samples/non_existing.txt", export_stream), std::invalid_argument);
  export_stream.close();
}

TEST_F(ExportTest, ExportDirectory) {
  std::ofstream export_stream("exported.txt");
  if (!export_stream.is_open()) throw std::runtime_error("Cannot open file exported.txt");
  EXPECT_THROW(file_system_.export_file("samples", export_stream), std::invalid_argument);
  export_stream.close();
}
