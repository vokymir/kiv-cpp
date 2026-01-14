#pragma once

#include "Point.hpp"
#include "Shape.hpp"
#include <stdexcept>
#include <string>

//  Class for any rectangle.
//  D ----- C
//  |       |
//  A ----- B
class Rectangle : public Shape {
private:
  Point A_;
  Point B_;
  Point C_;
  Point D_;

public:
  void translate(int x, int y) override;
  void rotate(int x, int y, float a) override;
  void scale(int x, int y, float f) override;

  void draw_svg(std::string &svg) const override;
  void draw_pgm(std::vector<std::vector<int>> &pixels) const override;

public:
  // CONSTRUCTORS
  Rectangle(int x, int y, int w, int h) {
    if (w <= 0) {
      throw std::runtime_error("TODO: rect must have >0 w");
    }
    if (h <= 0) {
      throw std::runtime_error("TODO: rect must have >0 h");
    }

    // (0,0) -- x
    // |
    // y
    A_.set(x, y);
    B_.set(x + w, y);
    C_.set(x + w, y + h);
    D_.set(x, y + h);
  }
};
