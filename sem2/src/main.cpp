#include "mpint.hpp"
#include "mpterm.hpp"
#include <cstddef>
#include <iostream>
#include <string_view>

// helper to avoid duplication
template <std::size_t P> void start_terminal() {
  MPTerm::MPTerm<P> term;
  term.run();
}

void print_usage() {
  std::cout << "Usage:\n./mpcalc.exe <mode>\n\nModes:\n1\tUnlimited "
               "precision\n2\t32 bytes precision\n3\tshowcase"
            << std::endl;
}

int main(int argc, char **argv) {
  if (argc < 2) {
    print_usage();
    return 1;
  }

  std::string_view mode = argv[1];

  if (mode == "1") {
    start_terminal<MPInt::Unlimited>();
  } else if (mode == "2") {
    start_terminal<32>();
  } else if (mode == "3") {
    // TODO: showcase
    std::cout << "Showcase is WIP" << std::endl;
  } else {
    std::cerr << "Error: Unknown mode '" << mode << "'\n";
    print_usage();
    return 2;
  }

  return 0;
}
