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

  Point() {}
  Point(int x, int y) : x_(x), y_(y) {}

  bool operator==(const Point &other) const {
    return x_ == other.x_ && y_ == other.y_;
  }

  void translate(int x, int y) {
    x_ += x;
    y_ += y;
  }

  void rotate(int x, int y, float a) {
    translate(-x, -y);

    float angle = a * std::numbers::pi / 180.0f;

    float old_x = static_cast<float>(x_);
    float old_y = static_cast<float>(y_);

    float new_x = old_x * std::cos(angle) - old_y * std::sin(angle);
    float new_y = old_x * std::sin(angle) + old_y * std::cos(angle);

    x_ = static_cast<int>(std::round(new_x));
    y_ = static_cast<int>(std::round(new_y));

    translate(x, y);
  }

  void scale(int x, int y, float f) {
    if (Util::is_zero(f)) {
      throw std::runtime_error("TODO: scale factor must != 0");
    }
    translate(-x, -y);

    x_ = static_cast<int>(std::round(x_ * f));
    y_ = static_cast<int>(std::round(y_ * f));

    translate(x, y);
  }
};
