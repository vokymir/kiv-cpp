
#include "Rectangle.hpp"
#include "Line.hpp"
#include "Point.hpp"
#include <string>
#include <vector>

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

std::string Rectangle::draw_svg() const {
  std::string output;

  // just to satisfy the lambda requirement
  auto draw_line = [](const Point &begin, const Point &end) {
    return Line(begin, end).draw_svg();
  };

  output += draw_line(A_, B_);
  output += draw_line(B_, C_);
  output += draw_line(C_, D_);
  output += draw_line(D_, A_);

  return output;
}

void Rectangle::draw_pgm(std::vector<std::vector<int>> &pixels) const {
  Line(A_, B_).draw_pgm(pixels);
  Line(B_, C_).draw_pgm(pixels);
  Line(C_, D_).draw_pgm(pixels);
  Line(D_, A_).draw_pgm(pixels);
}
