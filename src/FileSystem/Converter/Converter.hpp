#pragma once

#include <vector>

class Converter {
public:
  static auto to_bytes(std::uint64_t value) -> std::vector<std::byte>;
  static auto to_uint64(const std::vector<std::byte> &bytes) -> std::uint64_t;
};