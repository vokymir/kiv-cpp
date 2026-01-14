
#include "Point.hpp"
#include "Util.hpp"
#include <cmath>
#include <numbers>
#include <stdexcept>

void Point::translate(int x, int y) {
  x_ += x;
  y_ += y;
}

void Point::rotate(int x, int y, float a) {
  int dx = x_ - x;
  int dy = y_ - y;

  float angle = a * std::numbers::pi / 180.0f;

  float new_x = dx * std::cos(angle) - dy * std::sin(angle);
  float new_y = dx * std::sin(angle) + dy * std::cos(angle);

  x_ = new_x + x;
  y_ = new_y + y;
}

void Point::scale(int x, int y, float f) {
  if (Util::is_zero(f)) {
    throw std::runtime_error("TODO: scale factor must != 0");
  }

  x_ = x + (x_ - x) * f;
  y_ = y + (y_ - y) * f;
}
