#pragma once

// use fancy functionality of readline/ library on UNIX, but be compatible on
// windows simply by overlaying used functions from library with custom ones
// defined here
#ifdef _WIN32
// just a simple wrapper around getline (supported on WIN/UNIX), adhering to
// readline API, which returns char *
#define readline(prompt)                                                       \
  ([]() {                                                                      \
    std::string line;                                                          \
    std::cout << prompt;                                                       \
    std::getline(std::cin, line);                                              \
    char *cstr = new char[line.size() + 1];                                    \
    std::strcpy(cstr, line.c_str());                                           \
    return cstr;                                                               \
  }())

// don't do any history on WIN
#define add_history(x) ((void)0)
#else
#include <readline/history.h>
#include <readline/readline.h>
#endif

#include "mpint.hpp"
#include <algorithm>
#include <charconv>
#include <cstddef>
#include <deque>
#include <exception>
#include <iostream>
#include <regex>
#include <stdexcept>
#include <string>
#include <string_view>

// Implementation of Multiple Precision Terminal is in class MPTerm.
namespace MPTerm {

// All implementation details are hidden here not to be exposed to public API of
// namespace directly.
namespace _detail {

// bank for MPInt numbers with precision P, where atmost N numbers are stored
// allow pushing new numbers & getting at any index
// WARNING: uses 1-based array numbering
template <std::size_t P, std::size_t N> struct Bank {
private:
  // at lower indices are newer numbers
  std::deque<MPInt::MPInt<P>> bank_;

public:
  // push new number to bank - reordering of numbers happens & ensure max size N
  void push(MPInt::MPInt<P> &&number) {
    bank_.emplace_front(std::move(number));

    while (bank_.size() > N) {
      bank_.pop_back();
    }
  }

  // return const reference of number at given index
  // WARN: first number is at index 1
  const MPInt::MPInt<P> &get_const(std::size_t index) const {
    index -= 1; // convert to standard 0-based indexing
    if (index >= bank_.size()) {
      throw std::range_error("Access to bank at invalid index");
    }

    return bank_[index];
  }

  // return copy of number at given index
  // WARN: first number is at index 1
  MPInt::MPInt<P> get_copy(std::size_t index) const {
    const auto &ref = get_const(index);
    return MPInt::MPInt<P>(ref);
  }

  // return size of bank
  std::size_t size() const { return bank_.size(); }
};

// represents any supported operator AND invalid
enum class Operator { Add, Sub, Mul, Div, Fct, Invalid };

// convert operator to char
constexpr char op_to_char(Operator op) {
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
constexpr Operator op_from_char(char ch) {
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

// Multiple Precision Terminal with the precision specified as template
// parameter P. Uses MPInt numbers under the hood and provides the typical
// terminal functionality.
template <std::size_t P>
  requires(P >= 4 || P == MPInt::Unlimited)
class MPTerm {
  // ===== Variables =====
private:
  // storage for last 5 numbers
  _detail::Bank<P, 5> bank_;
  // flag which commands can manipulate
  bool running = false;

  // ===== Methods =====
public:
  // run the MP Terminal
  void run() {
    std::cout
        << "Welcome to Multiple Precision Terminal.\nCurrent precision is "
        << ((P == 0) ? "infinite" : std::to_string(P))
        << " bytes.\nWrite simple mathematical expression with at most one "
           "operation from +,-,*,/,! or the command 'bank' or 'exit'."
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

  // ===== Methods for handling input =====
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

  // === Commands ===

  // process command 'bank' and show bank in the terminal
  void cmd_show_bank() {
    auto print_num = [this](std::size_t i) {
      std::cout << "$" << i << " = " << bank_.get_const(i) << "\n";
    };

    for (std::size_t i = 1; i <= bank_.size(); ++i) {
      print_num(i);
    }
  }

  // process command 'exit' and exit the terminal
  void cmd_exit() { running = false; }

  // process mathematical expression
  void cmd_expression(const std::string &input) {
    // explanation of regex:
    // [spaces][$]number[spaces]operator[spaces][$]number[spaces]
    // where second number is optional (because of factorial)
    static const std::regex expression(
        R"(^\s*([$\d]+)\s*([+\-*/!])\s*([$\d]*)\s*$)");
    // (if there is $$1, it will fail)

    std::smatch match;
    if (!std::regex_match(input, match, expression)) {
      std::cout << "Invalid input." << std::endl;
      return;
    }

    // get operands & operator
    std::string op1 = match[1].str();
    _detail::Operator op = _detail::op_from_char(match[2].str()[0]);
    std::string op2 = match[3].str();

    // early fail on missing 2nd operator
    if (op != _detail::Operator::Fct && op2.empty()) {
      std::cout << "Second operand missing." << std::endl;
      return;
    }

    // do the maths - may throw for variety of reasons
    try {
      process_operation(op1, op, op2);

      // overflow is handled specifically according to the assignment
      // NOTE: won't prematurely return as other catches in this try
    } catch (const MPInt::Overflow_Error &e) {
      std::cout << "Operation caused overflow, because its result couldn't fit "
                   "inside terminal precision ("
                << ((P == 0) ? "infinite" : std::to_string(P))
                << " bytes).\nResult: " << e.unlimited_value()
                << "\nSaved as number with terminal precision." << std::endl;

      // store the number in terminal precision - using explicit constructor
      bank_.push(MPInt::MPInt<P>(e.unlimited_value()));

      // generic exception is just de facto ignored
    } catch (const std::exception &e) {
      std::cerr << e.what() << std::endl;
      std::cout << "Invalid input." << std::endl;
      return;

      // weird exception is just catched
    } catch (...) {
      std::cout << "Unknown error, presumably invalid input." << std::endl;
      return;
    }

    // show the result
    std::cout << "$1 = " << bank_.get_const(1) << std::endl;
  }

  // Get the actual numbers and do the math on them.
  // The second operand may be empty, if operator is factorial.
  //
  // May throw on any overflow and on any invalid operator.
  void process_operation(const std::string_view &op1, _detail::Operator op,
                         const std::string_view &op2) {
    MPInt::MPInt<P> result{};
    MPInt::MPInt<P> operand1 = get_operand(op1);

    // factorial is a special case - requires only 1 operand
    if (op == _detail::Operator::Fct) {
      result = std::move(operand1); // just so result is semantic
      result.factorial();
      bank_.push(std::move(result));
      return;
    }

    MPInt::MPInt<P> operand2 = get_operand(op2);

    // uses the operator overloads on MPInt
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
  // - operand is $x and the index is wrong/too big/small
  // - operand is a number, but it's too big for precision
  // NOTE: only in cmd_expression is that catched and handled as if the operand
  // is result. that's OK for positive numbers. might not be OK for negative
  // (eg. expression is 0 - verylongnumber), but that whole situation is so
  // rare and user is explicitely warned about error, that it could be
  // wavehanded
  MPInt::MPInt<P> get_operand(std::string_view str) {
    // normal number
    if (str[0] != '$') {
      return MPInt::MPInt<P>(str);
    }

    // number from bank
    int index = 0;
    auto idx_view = str.substr(1); // skip '$'

    // load number from idx_view to index. if that fails, error is in ec
    auto [ptr, ec] = std::from_chars(idx_view.data(),
                                     idx_view.data() + idx_view.size(), index);
    if (ec != std::errc{} || index <= 0) {
      throw std::invalid_argument("Invalid bank reference.");
    }

    // this needs to be modifiable instance of MPInt, so copy
    return bank_.get_copy(index);
  }

  // ===== Showcase =====

public:
  // show what this terminal is capable of
  void run_showcase() {
    std::vector<std::string> demo_expressions = {
        "123 + 456",        // simple addition
        "$1 * 2",           // multiplication using bank
        "100!",             // factorial
        "9876543210 / 123", // division with big number
        "-42 - 100",        // subtraction with negatives
        "$2 + $1",          // addition using two bank numbers
    };

    for (const auto &expr : demo_expressions) {
      std::cout << "> " << expr << "\n";
      cmd_expression(expr);
      std::cout << "\n";
    }

    std::cout << "\n=== Final Bank State ===\n";
    cmd_show_bank();
  }
};

} // namespace MPTerm
