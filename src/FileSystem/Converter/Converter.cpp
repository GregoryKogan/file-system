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

  if (bytes.size() != BYTES_IN_UINT64) throw std::invalid_argument("Invalid bytes size");

  std::uint64_t value = 0;

  for (std::size_t i = 0; i < BYTES_IN_UINT64; ++i) {
    value |= static_cast<std::uint64_t>(bytes[BYTES_IN_UINT64 - i - 1]) << BYTES_IN_UINT64 * i;
  }

  return value;
}

auto Converter::to_bytes(std::string const &value, std::uint64_t size) -> std::vector<std::byte> {
  std::vector<std::byte> bytes;
  for (char character : value) bytes.push_back(static_cast<std::byte>(character));

  if (size != 0) {
    if (bytes.size() > size) throw std::invalid_argument("Invalid string size");
    bytes.resize(size, std::byte{0});
  }

  return bytes;
}

auto Converter::to_string(std::vector<std::byte> const &bytes) -> std::string {
  std::string string;
  for (std::byte byte : bytes) {
    if (byte == std::byte{0}) break;
    string.push_back(static_cast<char>(byte));
  }

  return string;
}

auto Converter::to_bytes(bool value) -> std::vector<std::byte> {
  std::vector<std::byte> bytes;
  if (value) {
    bytes.push_back(std::byte{1});
  } else {
    bytes.push_back(std::byte{0});
  }
  return bytes;
}

auto Converter::to_bool(std::vector<std::byte> const &bytes) -> bool {
  if (bytes.size() != 1) throw std::invalid_argument("Invalid bytes size");

  if (bytes[0] == std::byte{0}) return false;
  if (bytes[0] == std::byte{1}) return true;
  throw std::invalid_argument("Invalid bool value");
}
