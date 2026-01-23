#pragma once

#include "mpint.hpp"
#include <algorithm>
#include <cstddef>
#include <deque>
#include <exception>
#include <iostream>
// library allowing usage of arrow up/down in terminal
#include <readline/history.h>
#include <readline/readline.h>
#include <stdexcept>
#include <string>
#include <string_view>

namespace MPTerm {

namespace _detail {

// bank for MPInt numbers with precision P, where atmost N numbers are stored
// allow pushing new numbers & getting at any index
// WARNING: uses 1-based array numbering
template <std::size_t P, std::size_t N> struct Bank {
  using mpint = MPInt::MPInt<P>;

private:
  // at lower indices are newer numbers
  std::deque<mpint> bank_;

public:
  // push new number to bank - reordering of numbers happens & ensure size N is
  // in place
  void push(mpint &&number) {
    bank_.emplace_front(std::move(number));

    while (bank_.size() > N) {
      bank_.pop_back();
    }
  }

  // return copy of number at given index
  // WARN: first number is at index 1
  mpint get(std::size_t index) const {
    index -= 1; // convert to standard 0-based indexing
    if (index >= bank_.size()) {
      throw std::range_error("Access to bank at invalid index");
    }

    return bank_[index];
  }

  // return size of bank
  std::size_t size() const { return bank_.size(); }
};

// represents any supported operator AND invalid
enum class Operator { Add, Sub, Mul, Div, Fct, Invalid };

// convert operator to char
constexpr char to_char(Operator op) {
  switch (op) {
  case Operator::Add:
    return '+';
  case Operator::Sub:
    return '-';
  case Operator::Mul:
    return '*';
  case Operator::Div:
    return '/';
  case Operator::Fct:
    return '!';

  case Operator::Invalid:
    return '?';
  }

  // also invalid operator, but in other way
  return '#';
}

// get operator from char
constexpr Operator from_char(char ch) {
  switch (ch) {
  case '+':
    return Operator::Add;
  case '-':
    return Operator::Sub;
  case '*':
    return Operator::Mul;
  case '/':
    return Operator::Div;
  case '!':
    return Operator::Fct;
  }
  return Operator::Invalid;
}

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
    std::cout
        << "Welcome to Multiple Precision Terminal.\nCurrent precision is "
        << ((P == 0) ? "infinite" : std::to_string(P))
        << " bytes.\nWrite simple mathematical expression with at most one "
           "operation from +.-,*,/,! or the command 'bank' or 'exit'."
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
  // determine what to do based on input, test all commands and if that fail try
  // expression
  void handle_input(const std::string &input) {
    if (input == "bank") {
      cmd_show_bank();

    } else if (input == "exit") {
      cmd_exit();

    } else {
      cmd_expression(input);
    }
  }

  // process command 'bank' and show bank in the terminal
  void cmd_show_bank() {
    for (std::size_t i = 1; i <= bank_.size(); ++i) {
      std::cout << "$" << std::to_string(i) << " = " << bank_.get(i) << "\n";
    }
  }

  // process command 'exit' and exit the terminal
  void cmd_exit() { running = false; }

  // process mathematical expression
  void cmd_expression(const std::string &input) {
    // find which operator was used
    auto op = get_operator(input);
    if (op == _detail::Operator::Invalid) {
      std::cout << "Invalid input." << std::endl;
      return;
    }

    std::size_t op_pos = input.find(_detail::to_char(op));
    if (op_pos == std::string::npos) {
      std::cout << "Invalid operator." << std::endl;
      return;
    }

    // load operands
    std::string_view op1 = get_first_operand_string(input, op_pos);

    std::string_view op2{};
    if (op != _detail::Operator::Fct) {
      op2 = get_second_operand_string(input, op_pos);
    }

    // do the maths - may throw for variety of reasons (one of which is invalid
    // formatting)
    try {
      process_operation(op1, op, op2);
    } catch (const std::exception &e) {
      std::cout << e.what() << std::endl;
      return;
    }

    catch (...) {
      std::cout << "Invalid input (make sure it has no spaces)." << std::endl;
      return;
    }

    // show the result
    std::cout << "$1 = " << bank_.get(1) << std::endl;
  }

  // get the first found operator in the string input by comparing all chars
  // (from left) to operators and wait until its not invalid
  _detail::Operator get_operator(const std::string &input) {
    _detail::Operator op = _detail::Operator::Invalid;

    for (std::size_t i = 0; i < input.size(); ++i) {
      op = _detail::from_char(input[i]);

      if (op != _detail::Operator::Invalid) {
        break;
      }
    }

    return op;
  }

  // get stringview on everything before the operand position
  std::string_view get_first_operand_string(std::string_view input,
                                            std::size_t op_pos) {
    return (op_pos < input.size()) ? input.substr(0, op_pos)
                                   : std::string_view{};
  }

  // get stringview on everything after the operand position
  std::string_view get_second_operand_string(std::string_view input,
                                             std::size_t op_pos) {
    return (op_pos + 1 < input.size()) ? input.substr(op_pos + 1)
                                       : std::string_view{};
  }

  // Get the actual numbers and do the math on them.
  // may throw on any get_operand() mismatch
  // and on any overflow
  // and on any invalid operator
  void process_operation(const std::string_view &op1, _detail::Operator op,
                         const std::string_view &op2) {
    MPInt::MPInt<P> result{};
    MPInt::MPInt<P> operand1 = get_operand(op1);

    // factorial is a special case - requires only 1 operand
    if (op == _detail::Operator::Fct) {
      result = operand1;
      result.factorial();
      bank_.push(std::move(result));
      return;
    }

    MPInt::MPInt<P> operand2 = get_operand(op2);

    switch (op) {
    case _detail::Operator::Add:
      result = operand1 + operand2;
      break;
    case _detail::Operator::Sub:
      result = operand1 - operand2;
      break;
    case _detail::Operator::Mul:
      result = operand1 * operand2;
      break;
    case _detail::Operator::Div:
      result = operand1 / operand2;
      break;
    default:
      throw std::runtime_error("Invalid operator .");
    }

    // save to bank
    bank_.push(std::move(result));
  }

  // get the actual MPInt from str. handle normal number and bank $x
  // may throw if:
  // - operand is a number, but it's too big for precision
  // - operand is $x and the index is wrong/too big/small
  MPInt::MPInt<P> get_operand(std::string_view str) {
    if (str.empty()) {
    }
    std::size_t pos = str.find('$');

    // there is no '$' => regular big number
    if (pos == std::string::npos) {
      return MPInt::MPInt<P>(str);
    }

    // get from bank at index which is right after $
    pos = std::min(pos + 1, str.size() - 1); // safe access
    int index = std::atoi(str.substr(pos + 1).data());
    return bank_.get(index);
  }
};

} // namespace MPTerm
