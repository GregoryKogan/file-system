#pragma once

#include "../Directory/Directory.hpp"
#include "../FileHandler/HandlerBuilder/HandlerBuilder.hpp"
#include <optional>
#include <ranges>
#include <string>
#include <vector>

class PathResolver {
  std::string delimiter_;
  HandlerBuilder handler_builder_;

public:
  PathResolver();
  PathResolver(std::string delimiter, HandlerBuilder handler_builder);

  [[nodiscard]] auto delimiter() const -> std::string const &;

  [[nodiscard]] auto search(std::string const &path, std::uint64_t search_dir) const -> std::optional<std::uint64_t>;
  [[nodiscard]] auto trace(std::uint64_t cluster) const -> std::string;

  [[nodiscard]] static auto dirname(std::string const &path, std::string const &delimiter) -> std::string;
  [[nodiscard]] static auto basename(std::string const &path, std::string const &delimiter) -> std::string;

  static auto parse(std::string const &path, std::string const &delimiter) -> std::vector<std::string>;

private:
  [[nodiscard]] auto get_file(std::vector<std::string> const &path_tokens, std::uint64_t file_cluster) const
      -> std::optional<std::uint64_t>;
  [[nodiscard]] auto find_file_in_dir(std::string const &file_name, Directory const &dir) const
      -> std::optional<std::uint64_t>;
};