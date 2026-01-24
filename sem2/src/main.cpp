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

// forward declare, defined after main to keep it high
void showcase();

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
    showcase();
  } else {
    std::cerr << "Error: Unknown mode '" << mode << "'\n";
    print_usage();
    return 2;
  }

  return 0;
}

void showcase() {
  std::cout << "=== MP Terminal Showcase ===\n"
            << "Demonstrating unlimited & fixed precision, arithmetic, "
               "factorial, and bank usage.\n"
            << std::endl;
  ;

  // run a terminal of any precision
  auto run_demo = [](auto prec, const std::string &label) {
    std::cout << "\n~~~ " << label << " ~~~\n" << std::endl;

    // template parameter deduced from type
    using TermType = decltype(prec);
    TermType term;
    term.run_showcase();
  };

  run_demo(MPTerm::MPTerm<MPInt::Unlimited>{}, "Unlimited precision terminal");
  run_demo(MPTerm::MPTerm<4>{}, "4-bytes precision terminal");
}
