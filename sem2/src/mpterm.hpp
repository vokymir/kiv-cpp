#pragma once

#include "mpint.hpp"
#include <algorithm>
#include <cstddef>
#include <deque>
#include <iostream>
// library allowing usage of arrow up/down in terminal
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
  mpint get(std::size_t index) const {
    if (index >= bank_.size()) {
      throw std::range_error("Access to bank at invalid index");
    }

    return bank_[index];
  }

  // return size of bank
  std::size_t size() const { return bank_.size(); }
};

} // namespace _detail

template <std::size_t P>
  requires(P >= 4 || P == MPInt::Unlimited)
class MPTerm {
private:
  _detail::Bank<P, 5> bank_;
  bool running = false;

public:
  // run the MP Terminal
  void run() {
    std::cout << "Welcome to Multiple Precision Terminal. Current precision is "
              << ((P == 0) ? "infinite" : std::to_string(P)) << " bytes."
              << std::endl;

    running = true;
    while (running) {
      char *input = readline("> ");

      if (!input) {
        break;
      }

      if (*input) {
        add_history(input);
        handle_input(input);
      }

      free(input);
    }

    std::cout << "Thank you for using the Multiple Precision Terminal."
              << std::endl;
  }

private:
  void handle_input(const std::string &input) {
    if (input == "bank") {
      cmd_show_bank();

    } else if (input == "exit") {
      cmd_exit();

    } else {
      cmd_expression(input);
    }
  }

  void cmd_show_bank() {
    for (std::size_t i = 0; i < bank_.size(); ++i) {
      std::cout << "$" << std::to_string(i) << " " << bank_.get(i) << "\n";
    }
  }

  void cmd_exit() { running = false; }

  void cmd_expression(const std::string &input) {}
};

} // namespace MPTerm
