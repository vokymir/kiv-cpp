
#include "Circle.hpp"
#include <format>
#include <string>

void Circle::translate(int x, int y) { center_.translate(x, y); }

void Circle::rotate(int x, int y, float a) { center_.rotate(x, y, a); }

void Circle::scale(int x, int y, float f) {
  center_.scale(x, y, f);
  // don't have to check f==0 -> that must have been done in scale
  radius_ *= f;
}

std::string Circle::draw_svg() {
  return std::format(
      R"(<circle cx="{}" cy="{}" r="{}" stroke="black" stroke-width="2" />
)",
      center_.x(), center_.y(), radius_);
}

std::string Circle::draw_pgm() { return "PGM CIRCLE"; }
