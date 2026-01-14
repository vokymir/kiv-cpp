
#include "Rectangle.hpp"

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

std::string Rectangle::draw_svg() { return "SVG rect"; }

std::string Rectangle::draw_pgm() { return "PGM rect"; }
