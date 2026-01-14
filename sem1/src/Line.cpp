
#include "Line.hpp"
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

std::string Line::draw_svg() { return "svg"; }
std::string Line::draw_pgm() { return "pgm"; }
