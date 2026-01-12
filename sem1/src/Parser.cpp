
#include "Parser.hpp"
#include "Canvas.hpp"
#include "Circle.hpp"
#include "Line.hpp"
#include <array>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>

bool Parser::parse_row(const std::string &row, Canvas &c) {
  std::istringstream iss(row);
  std::string token;

  read_token(iss, token);
  if (token.empty()) {
    // empty line, maybe just a comment
    return false;
  } else if (token == "line") {
    parse_line_cmd(iss, c);
  } else if (token == "circle") {
    parse_circle_cmd(iss, c);
  } else if (token == "rect") {
    parse_rect_cmd(iss, c);
  } else if (token == "translate") {
    parse_translate_cmd(iss, c);
  } else if (token == "rotate") {
    parse_rotate_cmd(iss, c);
  } else if (token == "scale") {
    parse_scale_cmd(iss, c);
  } else {
    throw std::runtime_error("TODO: invalid line in input file");
  }

  return true;
}

int Parser::parse_file(std::ifstream &f, Canvas &c) {
  if (!f) {
    throw std::runtime_error("Failed to open file for read.");
  }

  std::string row;
  int n_rows = 0;

  while (std::getline(f, row)) {
    if (parse_row(row, c)) {
      n_rows++;
    }
  }

  return n_rows;
}

void Parser::parse_line_cmd(std::istringstream &iss, Canvas &c) {
  std::array<int, 4> arr;
  for (int i = 0; i < 4; i++) {
    read_token(iss, arr[i]);
  }

  c.add_shape(std::make_unique<Line>(arr));
}

void Parser::parse_circle_cmd(std::istringstream &iss, Canvas &c) {
  int x, y;
  float r;

  read_token(iss, x);
  read_token(iss, y);
  read_token(iss, r);

  c.add_shape(std::make_unique<Circle>(x, y, r));
}
