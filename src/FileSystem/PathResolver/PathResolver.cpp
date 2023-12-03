#include "PathResolver.hpp"

PathResolver::PathResolver(std::string delimiter, std::shared_ptr<DiskReader> disk_reader, std::shared_ptr<FAT> fat,
                           std::uint64_t cluster_size)
    : delimiter_(std::move(delimiter)), disk_reader_(std::move(disk_reader)), fat_(std::move(fat)),
      cluster_size_(cluster_size) {}

auto PathResolver::delimiter() const -> std::string const & { return delimiter_; }

auto PathResolver::search(std::string const &path, FileData const &working_dir, FileData const &root_dir) const
    -> std::optional<FileData> {
  auto path_tokens = parse(path, delimiter_);

  if (path_tokens[0].empty()) {
    return get_file(std::vector<std::string>(path_tokens.begin() + 1, path_tokens.end()), root_dir);
  }
  return get_file(path_tokens, working_dir);
}

auto PathResolver::parse(std::string const &path, std::string const &delimiter) -> std::vector<std::string> {
  if (path == delimiter) return std::vector<std::string>{""};

  std::vector<std::string> result;
  std::string::size_type start = 0;
  std::string::size_type end = path.find(delimiter);

  while (end != std::string::npos) {
    result.push_back(path.substr(start, end - start));
    start = end + delimiter.size();
    end = path.find(delimiter, start);
  }

  result.push_back(path.substr(start, end));

  return result;
}

auto PathResolver::dirname(std::string const &path, std::string const &delimiter) -> std::string {
  if (path.empty()) throw std::invalid_argument("Path is empty");
  if (path == delimiter) return path;

  std::string path_copy = path;
  if (path_copy.find_last_of(delimiter) == path_copy.size() - delimiter.size()) {
    path_copy = path_copy.substr(0, path_copy.size() - delimiter.size());
  }

  auto last_delimiter = path_copy.find_last_of(delimiter);
  if (last_delimiter == std::string::npos) return ".";
  if (last_delimiter == 0) return delimiter;
  return path_copy.substr(0, last_delimiter);
}

auto PathResolver::basename(std::string const &path, std::string const &delimiter) -> std::string {
  if (path.empty()) throw std::invalid_argument("Path is empty");
  if (path == delimiter) return path;

  std::string path_copy = path;
  if (path_copy.find_last_of(delimiter) == path_copy.size() - delimiter.size()) {
    path_copy = path_copy.substr(0, path_copy.size() - delimiter.size());
  }

  return path_copy.substr(path_copy.find_last_of(delimiter) + 1);
}

auto PathResolver::get_file(std::vector<std::string> const &path_tokens, FileData const &file_data) const
    -> std::optional<FileData> {
  if (path_tokens.empty()) return file_data;
  if (!file_data.is_directory()) return {};

  auto file_reader = FileReader(file_data, FileHandler::FileOffset(0), fat_, cluster_size_, disk_reader_,
                                FileReader::BlockSize(file_data.size().bytes));
  auto cur_dir = Directory::from_bytes(file_reader.read_block());
  auto next = cur_dir.find(path_tokens[0]);
  if (!next) return {};
  return get_file(std::vector<std::string>(path_tokens.begin() + 1, path_tokens.end()), *next);
}
