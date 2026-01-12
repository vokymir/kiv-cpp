#pragma once

#include "Canvas.hpp"
#include <fstream>
#include <optional>
#include <sstream>
#include <string>

class Parser {
private:
  static void parse_row(const std::string &row, Canvas &c);
  static std::optional<std::string> get_next_token(std::istringstream &iss);
  static void parse_line_cmd(std::istringstream &iss, Canvas &c);
  static void parse_circle_cmd(std::istringstream &iss, Canvas &c);
  static void parse_rect_cmd(std::istringstream &iss, Canvas &c);
  static void parse_translate_cmd(std::istringstream &iss, Canvas &c);
  static void parse_rotate_cmd(std::istringstream &iss, Canvas &c);
  static void parse_scale_cmd(std::istringstream &iss, Canvas &c);

public:
  // Parse given source file from begin to end, line-by-line. Perform found
  // operations on given canvas.
  static void parse_file(std::ifstream &f, Canvas &c);
};
