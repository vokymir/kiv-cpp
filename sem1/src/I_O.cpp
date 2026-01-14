
#include "I_O.hpp"
#include "Canvas.hpp"
#include "Circle.hpp"
#include "Line.hpp"
#include "Rectangle.hpp"
#include <array>
#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>

bool I_O::parse_row(const std::string &row, Canvas &c) {
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
    throw std::runtime_error("Invalid command in input file");
  }

  return true;
}

int I_O::parse_file(const std::filesystem::path &source, Canvas &c) {
  std::ifstream file{source};
  if (!file) {
    throw std::runtime_error("Cannot open input file");
  }

  std::string row;
  int nth_row = 1; // for error output only
  int n_rows = 0;  // count valid rows

  // process all lines & count valid ones
  try {

    while (std::getline(file, row)) {
      if (parse_row(row, c)) {
        n_rows++;
      }
      nth_row++;
    }

  } catch (const std::exception &e) {
    throw std::runtime_error(std::format("Line {}: {}", nth_row, e.what()));

  } catch (...) {
    throw;
  }

  return n_rows;
}

void I_O::parse_line_cmd(std::istringstream &iss, Canvas &c) {
  std::array<int, 4> arr;
  for (int i = 0; i < 4; i++) {
    read_token(iss, arr[i]);
  }

  c.add_shape(std::make_unique<Line>(arr));
}

void I_O::parse_circle_cmd(std::istringstream &iss, Canvas &c) {
  int x, y;
  float r;

  read_token(iss, x);
  read_token(iss, y);
  read_token(iss, r);

  c.add_shape(std::make_unique<Circle>(x, y, r));
}

void I_O::parse_rect_cmd(std::istringstream &iss, Canvas &c) {
  int x, y, w, h;

  read_token(iss, x);
  read_token(iss, y);
  read_token(iss, w);
  read_token(iss, h);

  c.add_shape(std::make_unique<Rectangle>(x, y, w, h));
}

void I_O::parse_translate_cmd(std::istringstream &iss, Canvas &c) {
  int x, y;

  read_token(iss, x);
  read_token(iss, y);

  c.translate(x, y);
}

void I_O::parse_rotate_cmd(std::istringstream &iss, Canvas &c) {
  int x, y;
  float a;

  read_token(iss, x);
  read_token(iss, y);
  read_token(iss, a);

  c.rotate(x, y, a);
}

void I_O::parse_scale_cmd(std::istringstream &iss, Canvas &c) {
  int x, y;
  float f;

  read_token(iss, x);
  read_token(iss, y);
  read_token(iss, f);

  c.scale(x, y, f);
}

void I_O::write(const Canvas &canvas, std::filesystem::path &target) {
  std::ofstream target_file{target};
  if (!target_file) {
    throw std::runtime_error("Cannot write to output file");
  }

  if (target.extension() == ".svg") {
    target_file << canvas.draw_svg();

  } else if (target.extension() == ".pgm") {
    target_file << canvas.draw_pgm();

  } else {
    throw std::runtime_error("Unsupported output file extension (" +
                             target.extension().string() + ").");
  }
}
