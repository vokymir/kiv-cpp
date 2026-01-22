#pragma once

#include "mpint.hpp"
#include <algorithm>
#include <cstddef>
#include <deque>
#include <iostream>
#include <readline/history.h>
#include <readline/readline.h>
#include <stdexcept>
#include <string>

namespace MPTerm {

namespace _detail {

// bank for MPInt numbers with precision P, where atmost N numbers are stored
// allow pushing new numbers & getting at any index
template <std::size_t P, std::size_t N> struct Bank {
  using mpint = MPInt::MPInt<P>;

private:
  // at lower indices are newer numbers
  std::deque<mpint> bank_;

public:
  // push new number to bank - reordering of numbers happens
  void push(mpint &&number) {
    while (bank_.size() > N) {
      bank_.pop_back();
    }

    bank_.emplace_front(std::move(number));
  }

  // return copy of number at given index
  mpint get(std::size_t index) {
    if (index >= bank_.size()) {
      throw std::range_error("Access to bank at invalid index");
    }

    return bank_[index];
  }
};

} // namespace _detail

template <std::size_t P>
  requires(P >= 4 || P == MPInt::Unlimited)
class MPTerm {
private:
  _detail::Bank<P, 5> bank_;

public:
  // run the MP Terminal
  void run() {
    std::cout << "Welcome to Multiple Precision Terminal. Current precision is "
              << ((P == 0) ? "infinite" : std::to_string(P)) << " bytes."
              << std::endl;

    bool exit = false;
    while (!exit) {
      char *input = readline("> ");

      if (!input) {
        break;
      }

      if (*input) {
        add_history(input);
      }

      free(input);
    }

    std::cout << "Thank you for using the Multiple Precision Terminal."
              << std::endl;
  }
};

} // namespace MPTerm
