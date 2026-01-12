#pragma once

#include "Point.hpp"
#include "Shape.hpp"
#include <stdexcept>

class Circle : public Shape {
private:
  Point p1_;
  float r_ = 1;

public:
  void translate(int x, int y) override;
  void rotate(int x, int y, float a) override;
  void scale(int x, int y, float f) override;

  std::string draw_svg() override;
  std::string draw_pgm() override;

public:
  // CONSTRUCTORS
  Circle(int x, int y, float r) : p1_(x, y), r_(r) {
    if (r_ <= 0) {
      throw std::runtime_error("TODO: radius must be > 0");
    }
  }

  Circle() : Circle(0, 0, 1) {}

  Circle(const Point &p1, float r) : Circle(p1.x(), p1.y(), r) {};
};
