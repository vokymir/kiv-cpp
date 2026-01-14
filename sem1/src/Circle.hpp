#pragma once

#include "Point.hpp"
#include "Shape.hpp"
#include <stdexcept>
#include <string>

// Specific shape - defined by its center point and radius.
class Circle : public Shape {
private:
  Point center_;
  float radius_ = 1;

public:
  void translate(int x, int y) override;
  void rotate(int x, int y, float a) override;
  void scale(int x, int y, float f) override;

  // Append standard SVG circle tag. Specs of circle as in the assignment.
  void draw_svg(std::string &svg) const override;

  // Draw black circle inside the *2D array*. Uses [Midpoint circle
  // algorithm](https://en.wikipedia.org/wiki/Midpoint_circle_algorithm) to
  // calculate which pixels to fill.
  void draw_pgm(std::vector<std::vector<int>> &pixels) const override;

  // CONSTRUCTORS
public:
  // Create new circle with its center located at (x,y) and (positive!) radius
  // r.
  Circle(int x, int y, float r) : center_(x, y), radius_(r) {
    if (radius_ <= 0) {
      throw std::runtime_error("TODO: radius must be > 0");
    }
  }

  Circle() : Circle(0, 0, 1) {}

  Circle(const Point &p1, float r) : Circle(p1.x(), p1.y(), r) {};
};
