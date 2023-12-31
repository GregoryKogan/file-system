#pragma once

#include <functional>
#include <sstream>
#include <string>
#include <vector>

class Error { // NOLINT (bugprone-exception-escape)
  std::string message_;
  std::function<std::string()> get_type_ = []() { return "Error"; };
  std::vector<Error> trace_;

public:
  explicit Error(std::string message) : message_(std::move(message)) {}
  Error(std::string message, const std::function<std::string()> &get_type)
      : message_(std::move(message)), get_type_(get_type) {}

  [[nodiscard]] auto type() const -> std::string { return get_type_(); }
  [[nodiscard]] auto what() const -> std::string { return type() + ": " + message_; }
  [[nodiscard]] auto trace() const -> std::vector<Error> {
    auto trace = trace_;
    trace.push_back(*this);
    return trace;
  }
  auto log(std::ostream &out_stream) const -> void {
    for (auto const &error : trace()) { out_stream << "  " << error.what() << '\n'; }
  }

  auto wrap(const Error &error) -> Error & {
    auto trace = error.trace();
    trace.insert(std::end(trace), std::begin(trace_), std::end(trace_));
    trace_ = trace;
    return *this;
  }
};

#define DEFINE_ERROR(name) /* NOLINT (cppcoreguidelines-macro-usage) */                                                \
  class name##Error : public Error {                                                                                   \
  public:                                                                                                              \
    explicit name##Error(std::string message) : Error(std::move(message), [] { return #name; }) {}                     \
  };