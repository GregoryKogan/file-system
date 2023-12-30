#pragma once

#include <string>
#include <utility>

struct Error {
  virtual ~Error() = default;
  Error() = default;
  Error(const Error &) = default;
  Error(Error &&) = default;
  auto operator=(const Error &) -> Error & = delete;
  auto operator=(Error &&) -> Error & = delete;

  explicit Error(std::string in_message) : message(std::move(in_message)) {}
  [[nodiscard]] auto get_error_type() const noexcept -> std::string { // NOLINT (bugprone-exception-escape)
    return "Error";
  }

  [[nodiscard]] auto what() const noexcept -> std::string { return get_error_type() + ": " + message; }

private:
  std::string message;
};

#define DEFINE_ERROR_TYPE(name) /* NOLINT (cppcoreguidelines-macro-usage) */                                           \
  struct name##Err : public Error {                                                                                    \
    explicit name##Err(std::string in_message) : Error(std::move(in_message)) {}                                       \
    [[nodiscard]] auto get_error_type() const noexcept /* NOLINT (bugprone-exception-escape) */                        \
        -> std::string {                                                                                               \
      return #name;                                                                                                    \
    }                                                                                                                  \
  };
