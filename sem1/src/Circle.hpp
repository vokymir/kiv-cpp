#pragma once

#include "Point.hpp"
#include "Shape.hpp"
#include <stdexcept>
#include <string>

class Circle : public Shape {
private:
  Point center_;
  float radius_ = 1;

public:
  void translate(int x, int y) override;
  void rotate(int x, int y, float a) override;
  void scale(int x, int y, float f) override;

  void draw_svg(std::string &svg) const override;
  void draw_pgm(std::vector<std::vector<int>> &pixels) const override;

public:
  // CONSTRUCTORS
  Circle(int x, int y, float r) : center_(x, y), radius_(r) {
    if (radius_ <= 0) {
      throw std::runtime_error("TODO: radius must be > 0");
    }
  }

  Circle() : Circle(0, 0, 1) {}

  Circle(const Point &p1, float r) : Circle(p1.x(), p1.y(), r) {};
};
