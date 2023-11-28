#include "Converter.hpp"

auto Converter::to_bytes(std::uint64_t value) -> std::vector<std::byte> {
  const uint8_t BYTES_IN_UINT64 = 8;
  const uint8_t BITS_IN_BYTE = 8;

  std::vector<std::byte> bytes;
  bytes.resize(BYTES_IN_UINT64);

  for (std::size_t i = 0; i < BYTES_IN_UINT64; ++i) {
    bytes[BYTES_IN_UINT64 - i - 1] = static_cast<std::byte>(value >> BITS_IN_BYTE * i);
  }

  return bytes;
}

auto Converter::to_uint64(const std::vector<std::byte> &bytes) -> std::uint64_t {
  const uint8_t BYTES_IN_UINT64 = 8;

  if (bytes.size() != BYTES_IN_UINT64) throw std::runtime_error("Invalid bytes size");

  std::uint64_t value = 0;

  for (std::size_t i = 0; i < BYTES_IN_UINT64; ++i) {
    value |= static_cast<std::uint64_t>(bytes[BYTES_IN_UINT64 - i - 1]) << BYTES_IN_UINT64 * i;
  }

  return value;
}
