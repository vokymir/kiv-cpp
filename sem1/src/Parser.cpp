
#include "Parser.hpp"
#include "Canvas.hpp"
#include "Point.hpp"
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

void Parser::parse_row(const std::string &row, Canvas &c) {
  std::istringstream iss(row);
  std::string token;

  read_token(iss, token);
  if (token == "line") {
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
  } else { // TODO: nicer output
    std::cout << "WEIRD_LINE" << std::endl;
  }
}

void Parser::parse_file(std::ifstream &f, Canvas &c) {
  if (!f) {
    throw std::runtime_error("Failed to open file for read.");
  }

  std::string row;
  while (std::getline(f, row)) {
    parse_row(row, c);
  }
}

void Parser::parse_line_cmd(std::istringstream &iss, Canvas &c) {
  Point p1{};
  Point p2{};
}
