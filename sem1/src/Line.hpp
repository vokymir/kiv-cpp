#pragma once

#include "Point.hpp"
#include "Shape.hpp"
#include <array>

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

  Line(const Point &p1, const Point &p2) : p1_(p1), p2_(p2) {}
  Line(int x1, int y1, int x2, int y2) : p1_(x1, y1), p2_(x2, y2) {}
  Line(const std::array<int, 4> &arr)
      : p1_(arr[0], arr[1]), p2_(arr[2], arr[3]) {}
};
