
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

void Rectangle::draw_svg(std::string &svg) const {
  Line(A_, B_).draw_svg(svg);
  Line(B_, C_).draw_svg(svg);
  Line(C_, D_).draw_svg(svg);
  Line(D_, A_).draw_svg(svg);
}

void Rectangle::draw_pgm(std::vector<std::vector<int>> &pixels) const {
  Line(A_, B_).draw_pgm(pixels);
  Line(B_, C_).draw_pgm(pixels);
  Line(C_, D_).draw_pgm(pixels);
  Line(D_, A_).draw_pgm(pixels);
}
