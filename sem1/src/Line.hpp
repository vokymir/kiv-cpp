#pragma once

#include "Point.hpp"
#include "Shape.hpp"
#include <array>
#include <stdexcept>

class Line : public Shape {
private:
  Point p1_;
  Point p2_;

public:
  void translate(int x, int y) override;
  void rotate(int x, int y, float a) override;
  void scale(int x, int y, float f) override;

  std::string draw_svg() override;
  std::string draw_pgm() override;

  // CONSTUCTORS
  Line(int x1, int y1, int x2, int y2) : p1_(x1, y1), p2_(x2, y2) {
    if (p1_ == p2_) {
      throw std::runtime_error(
          "TODO: line must have different origin from end");
    }
  }

  Line() : Line(0, 0, 1, 1) {}

  Line(const Point &p1, const Point &p2)
      : Line(p1.x(), p1.y(), p2.x(), p2.y()) {}

  Line(const std::array<int, 4> &arr) : Line(arr[0], arr[1], arr[2], arr[3]) {}
};
