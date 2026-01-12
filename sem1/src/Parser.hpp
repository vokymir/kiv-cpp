#pragma once

#include "Canvas.hpp"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>

class Parser {
private:
  // Return true if there was a valid statement, not only comment.
  static bool parse_row(const std::string &row, Canvas &c);
  static void parse_line_cmd(std::istringstream &iss, Canvas &c);
  static void parse_circle_cmd(std::istringstream &iss, Canvas &c);
  static void parse_rect_cmd(std::istringstream &iss, Canvas &c);
  static void parse_translate_cmd(std::istringstream &iss, Canvas &c);
  static void parse_rotate_cmd(std::istringstream &iss, Canvas &c);
  static void parse_scale_cmd(std::istringstream &iss, Canvas &c);

public:
  // Parse given source file from begin to end, line-by-line. Perform found
  // operations on given canvas. Return number of successfully parsed rows.
  static int parse_file(std::ifstream &f, Canvas &c);

private:
  // template

  template <typename T>
  static void read_token(std::istringstream &iss, T &out) {
    std::string token;

    // read token, exit if doesn't exist
    if (!(iss >> token) || token.empty()) {
      throw std::runtime_error("TODO");
    }

    // handle comment
    auto hashmark_pos = token.find('#');
    if (hashmark_pos != std::string::npos) {
      token = token.substr(0, hashmark_pos);
    }

    std::istringstream conv(token);
    if (!(conv >> out)) {
      throw std::runtime_error("TODO");
    }
  }
};
