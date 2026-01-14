
#include "Rectangle.hpp"
#include "Point.hpp"
#include <format>
#include <string>

void Rectangle::translate(int x, int y) {
  A_.translate(x, y);
  B_.translate(x, y);
  C_.translate(x, y);
  D_.translate(x, y);
}

void Rectangle::rotate(int x, int y, float a) {
  A_.rotate(x, y, a);
  B_.rotate(x, y, a);
  C_.rotate(x, y, a);
  D_.rotate(x, y, a);
}

void Rectangle::scale(int x, int y, float f) {
  A_.scale(x, y, f);
  B_.scale(x, y, f);
  C_.scale(x, y, f);
  D_.scale(x, y, f);
}

std::string Rectangle::draw_svg() {
  std::string output;

  auto draw_line = [](const Point &begin, const Point &end) {
    return std::format(
        R"(<line x1="{}" y1="{}" x2="{}" y2="{}" stroke="black" stroke-width="2" />
)",
        begin.x(), begin.y(), end.x(), end.y());
  };

  output += draw_line(A_, B_);
  output += draw_line(B_, C_);
  output += draw_line(C_, D_);
  output += draw_line(D_, A_);

  return output;
}

std::string Rectangle::draw_pgm() { return "PGM rect"; }
