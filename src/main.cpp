#include "CLI/CLI.hpp"

auto main() -> int {
  try {
    CLI cli;
    cli.run();
  } catch (std::exception const &e) { std::cerr << e.what() << '\n'; }
}
