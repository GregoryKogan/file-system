#pragma once

#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

class Converter {
  static const std::uint64_t UINT64_SIZE = 8;

public:
  // uint64_t
  [[nodiscard]] static auto to_bytes(std::uint64_t value) -> std::vector<std::byte>;
  [[nodiscard]] static auto to_uint64(const std::vector<std::byte> &bytes) -> std::uint64_t;
  [[nodiscard]] static auto get_uint64_size() noexcept -> std::uint64_t;

  // string
  [[nodiscard]] static auto to_bytes(std::string const &value, std::uint64_t size = 0) -> std::vector<std::byte>;
  [[nodiscard]] static auto to_string(std::vector<std::byte> const &bytes) -> std::string;

  // bool
  [[nodiscard]] static auto to_bytes(bool value) -> std::vector<std::byte>;
  [[nodiscard]] static auto to_bool(std::vector<std::byte> const &bytes) -> bool;
};