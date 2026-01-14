#pragma once

#include "Util.hpp"
#include <cmath>
#include <numbers>
#include <stdexcept>
class Point {
private:
  int x_;
  int y_;

public:
  int x() const { return x_; }
  void x(int x) { x_ = x; }

  int y() const { return y_; }
  void y(int y) { y_ = y; }

  void set(int x, int y) {
    x_ = x;
    y_ = y;
  }

  Point() : x_(0), y_(0) {}
  Point(int x, int y) : x_(x), y_(y) {}

  bool operator==(const Point &other) const {
    return x_ == other.x_ && y_ == other.y_;
  }

  void translate(int x, int y) {
    x_ += x;
    y_ += y;
  }

  void rotate(int x, int y, float a) {
    int dx = x_ - x;
    int dy = y_ - y;

    float angle = a * std::numbers::pi / 180.0f;

    float new_x = dx * std::cos(angle) - dy * std::sin(angle);
    float new_y = dx * std::sin(angle) + dy * std::cos(angle);

    x_ = new_x + x;
    y_ = new_y + y;
  }

  void scale(int x, int y, float f) {
    if (Util::is_zero(f)) {
      throw std::runtime_error("TODO: scale factor must != 0");
    }

    x_ = x + (x_ - x) * f;
    y_ = y + (y_ - y) * f;
  }
};
