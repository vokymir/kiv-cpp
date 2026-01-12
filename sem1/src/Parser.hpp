#pragma once

#include "Canvas.hpp"
#include <fstream>
#include <string>

class Parser {
private:
  static void parse_line(const std::string &line, Canvas &c);

public:
  // Parse given source file from begin to end, line-by-line. Perform found
  // operations on given canvas.
  static void parse_file(const std::ifstream &f, Canvas &c);
};
