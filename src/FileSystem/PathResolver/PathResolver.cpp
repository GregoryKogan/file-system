#include "PathResolver.hpp"

PathResolver::PathResolver() : delimiter_("/") {}

PathResolver::PathResolver(std::string delimiter, HandlerBuilder handler_builder)
    : delimiter_(std::move(delimiter)), handler_builder_(std::move(handler_builder)) {}

auto PathResolver::delimiter() const -> std::string const & { return delimiter_; }

auto PathResolver::search(std::string const &path, std::uint64_t search_dir) const -> std::optional<std::uint64_t> {
  auto path_tokens = parse(path, delimiter_);

  if (path_tokens[0].empty()) {
    // if path starts with delimiter, search from root
    return get_file(std::vector<std::string>(path_tokens.begin() + 1, path_tokens.end()), 0);
  }
  return get_file(path_tokens, search_dir);
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

auto PathResolver::get_file(std::vector<std::string> const &path_tokens, std::uint64_t file_cluster) const
    -> std::optional<std::uint64_t> {
  if (path_tokens.empty()) return file_cluster;

  auto metadata_handler = handler_builder_.build_metadata_handler(file_cluster);
  auto file_data = metadata_handler.read_metadata();
  if (!file_data.is_directory()) return {};

  if (path_tokens[0] == ".")
    return get_file(std::vector<std::string>(path_tokens.begin() + 1, path_tokens.end()), file_cluster);
  if (path_tokens[0] == "..") {
    return get_file(std::vector<std::string>(path_tokens.begin() + 1, path_tokens.end()),
                    file_data.get_parent_first_cluster());
  }

  auto file_reader = handler_builder_.build_file_reader(file_cluster);
  file_reader.set_block_size(file_data.get_size());

  auto cur_dir = Directory::from_bytes(file_reader.read());
  auto next = find_file_in_dir(path_tokens[0], cur_dir);
  if (!next) return {};
  return get_file(std::vector<std::string>(path_tokens.begin() + 1, path_tokens.end()), *next);
}

auto PathResolver::find_file_in_dir(std::string const &file_name, Directory const &dir) const
    -> std::optional<std::uint64_t> {
  for (auto const &cluster : dir.list_files()) {
    auto metadata_handler = handler_builder_.build_metadata_handler(cluster);
    auto file_data = metadata_handler.read_metadata();
    if (file_data.get_name() == file_name) return cluster;
  }

  return {};
}
