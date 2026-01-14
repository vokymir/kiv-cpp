#pragma once

#include "Canvas.hpp"
#include <filesystem>
#include <sstream>
#include <stdexcept>
#include <string>

// Class for every input/output operation related to canvas.
// Main functionality:
// + parse source file & update canvas accordingly
// - write existing canvas to target file
class I_O {
  // INPUT
private:
  // Return true if there was a valid statement, not only comment.
  // Only check first token and based on that delegate the work to specialized
  // functions.
  static bool parse_row(const std::string &row, Canvas &c);

  // Create Line from next 4 tokens, or throw.
  // Expected tokens: x1, y1, x2, y2
  static void parse_line_cmd(std::istringstream &iss, Canvas &c);
  // Create Circle from next 3 tokens, or throw.
  // Expected tokens: x, y, r
  static void parse_circle_cmd(std::istringstream &iss, Canvas &c);
  // Create Rectangle from next 4 tokens, or throw.
  // Expected tokens: x, y, w, h
  // (x,y) being the upper-left corner
  static void parse_rect_cmd(std::istringstream &iss, Canvas &c);

  // Apply translation based on next 2 tokens.
  // Expected tokens: x, y
  static void parse_translate_cmd(std::istringstream &iss, Canvas &c);
  // Apply rotation based on next 3 tokens.
  // Expected tokens: x, y, a
  // (x,y) being the center of rotation
  // a is the angle in **degrees**
  static void parse_rotate_cmd(std::istringstream &iss, Canvas &c);
  // Apply scale based on next 3 tokens.
  // Expected tokens: x, y, f
  // (x,y) being the center of scaling
  // f is the scaling factor
  static void parse_scale_cmd(std::istringstream &iss, Canvas &c);

public:
  // Parse given source file from begin to end, line-by-line. Perform found
  // operations on given canvas. Return number of successfully parsed rows. May
  // throw.
  static int parse_file(const std::filesystem::path &source, Canvas &c);

  // OUTPUT
public:
  // Write a canvas to a target file. Uses SVG or PGM based on target.extension.
  // May throw.
  static void write(const Canvas &canvas, std::filesystem::path &target);

  // INPUT (template)
private:
  // Read the next token from <iss> into <out>. Skip comments. May throw on
  // incompatible type.
  //
  // 3 possibilities with comments:
  // 1. no comment = no problem
  // 2. comment symbol (#) on the beginning of line = whole row skipped in
  // parse_row
  // 3. comment symbol somewhere else = if comment doesn't interfere with row
  // command, all good. If does, it's invalid syntax and throwed.
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

    // set out variable
    std::istringstream conv(token);
    if (!(conv >> out)) {
      throw std::runtime_error("TODO");
    }
  }
};
