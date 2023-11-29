#pragma once

#include <string>
#include <vector>

class Converter {
public:
  // uint64_t
  static auto to_bytes(std::uint64_t value) -> std::vector<std::byte>;
  static auto to_uint64(const std::vector<std::byte> &bytes) -> std::uint64_t;

  // string
  static auto to_bytes(std::string const &value, std::uint64_t size = 0) -> std::vector<std::byte>;
  static auto to_string(std::vector<std::byte> const &bytes) -> std::string;

  // bool
  static auto to_bytes(bool value) -> std::vector<std::byte>;
  static auto to_bool(std::vector<std::byte> const &bytes) -> bool;
};