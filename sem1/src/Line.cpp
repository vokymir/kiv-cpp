
#include "Line.hpp"
#include <format>
#include <string>

void Line::translate(int x, int y) {
  p1_.translate(x, y);
  p2_.translate(x, y);
}

void Line::rotate(int x, int y, float a) {
  p1_.rotate(x, y, a);
  p2_.rotate(x, y, a);
}

void Line::scale(int x, int y, float f) {
  p1_.scale(x, y, f);
  p2_.scale(x, y, f);
}

std::string Line::draw_svg() {
  return std::format(
      R"(<line x1="{}" y1="{}" x2="{}" y2="{}" stroke="black" />\n)", p1_.x(),
      p1_.y(), p2_.x(), p2_.y());
}
std::string Line::draw_pgm() { return "pgm"; }
