
#include "Parser.hpp"
#include "Canvas.hpp"
#include <iostream>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>

void Parser::parse_row(const std::string &row, Canvas &c) {
  std::istringstream iss(row);
  std::string token;

  // read token, exit if doesn't exist
  if (!(iss >> token)) {
    return;
  }
  // comment row only
  if (!token.empty() && token.find('#') != std::string::npos) {
    return;
  }

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

std::optional<std::string> Parser::get_next_token(std::istringstream &iss) {
  std::string token;

  // read token, exit if doesn't exist
  if (!(iss >> token) || token.empty()) {
    return std::nullopt;
  }

  // handle comment
  auto hashmark_pos = token.find('#');
  if (hashmark_pos != std::string::npos) {
    return token.substr(0, hashmark_pos);
  }

  return token;
}

void Parser::parse_file(std::ifstream &f, Canvas &c) {
  if (!f) {
    throw std::runtime_error("Failed to open file for read.");
  }

  std::string line;
  while (std::getline(f, line)) {
    parse_row(line, c);
  }
}
