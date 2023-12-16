#include "../src/FileSystem/Converter/Converter.hpp"
#include <gtest/gtest.h>

TEST(ConverterTest, ToBytesUint64) {
  std::uint64_t const value = 0x0123456789ABCDEF;
  std::vector<std::byte> const expected = {
      std::byte{0x01}, std::byte{0x23}, std::byte{0x45}, std::byte{0x67},
      std::byte{0x89}, std::byte{0xAB}, std::byte{0xCD}, std::byte{0xEF},
  };

  auto const actual = Converter::to_bytes(value);

  EXPECT_EQ(actual, expected);
}

TEST(ConverterTest, ToUint64) {
  std::vector<std::byte> const bytes = {
      std::byte{0x01}, std::byte{0x23}, std::byte{0x45}, std::byte{0x67},
      std::byte{0x89}, std::byte{0xAB}, std::byte{0xCD}, std::byte{0xEF},
  };
  std::uint64_t const expected = 0x0123456789ABCDEF;

  auto const actual = Converter::to_uint64(bytes);

  EXPECT_EQ(actual, expected);
}

TEST(ConverterTest, ToUint64InvalidSize) {
  std::vector<std::byte> const bytes = {
      std::byte{0xEF}, std::byte{0xCD}, std::byte{0xAB}, std::byte{0x89},
      std::byte{0x67}, std::byte{0x45}, std::byte{0x23},
  };

  EXPECT_THROW(auto res = Converter::to_uint64(bytes), std::invalid_argument);
}

TEST(ConverterTest, ToBytesString) {
  std::string const value = "Hello, World!";
  std::vector<std::byte> const expected = {
      std::byte{'H'}, std::byte{'e'}, std::byte{'l'}, std::byte{'l'}, std::byte{'o'}, std::byte{','}, std::byte{' '},
      std::byte{'W'}, std::byte{'o'}, std::byte{'r'}, std::byte{'l'}, std::byte{'d'}, std::byte{'!'}, std::byte{'\0'},
      std::byte{0},   std::byte{0},   std::byte{0},   std::byte{0},   std::byte{0},   std::byte{0},   std::byte{0}};

  auto const actual = Converter::to_bytes(value, 21);

  EXPECT_EQ(actual, expected);
}

TEST(ConverterTest, ToBytesStringInvalidSize) {
  std::string const value = "Hello, World!";

  EXPECT_THROW(auto res = Converter::to_bytes(value, 5), std::invalid_argument);
}

TEST(ConverterTest, ToString) {
  std::vector<std::byte> const bytes = {
      std::byte{'H'}, std::byte{'e'}, std::byte{'l'}, std::byte{'l'}, std::byte{'o'}, std::byte{','}, std::byte{' '},
      std::byte{'W'}, std::byte{'o'}, std::byte{'r'}, std::byte{'l'}, std::byte{'d'}, std::byte{'!'}, std::byte{'\0'},
      std::byte{0},   std::byte{0},   std::byte{0},   std::byte{0},   std::byte{0},   std::byte{0},   std::byte{0}};

  std::string const expected = "Hello, World!";

  auto const actual = Converter::to_string(bytes);

  EXPECT_EQ(actual, expected);
}

TEST(ConverterTest, ToBytesBool) {
  std::vector<std::byte> const expected_true = {std::byte{1}};
  std::vector<std::byte> const expected_false = {std::byte{0}};

  auto const actual_true = Converter::to_bytes(true);
  auto const actual_false = Converter::to_bytes(false);

  EXPECT_EQ(actual_true, expected_true);
  EXPECT_EQ(actual_false, expected_false);
}

TEST(ConverterTest, ToBool) {
  std::vector<std::byte> const bytes_true = {std::byte{1}};
  std::vector<std::byte> const bytes_false = {std::byte{0}};

  auto const actual_true = Converter::to_bool(bytes_true);
  auto const actual_false = Converter::to_bool(bytes_false);

  EXPECT_TRUE(actual_true);
  EXPECT_FALSE(actual_false);
}

TEST(ConverterTest, ToBoolInvalidSize) {
  std::vector<std::byte> const bytes = {std::byte{1}, std::byte{0}};

  EXPECT_THROW(auto res = Converter::to_bool(bytes), std::invalid_argument);
}

TEST(ConverterTest, ToBoolInvalidValue) {
  std::vector<std::byte> const bytes = {std::byte{2}};

  EXPECT_THROW(auto res = Converter::to_bool(bytes), std::invalid_argument);
}