#pragma once

#include "Point.hpp"
#include "Shape.hpp"
#include <stdexcept>
#include <string>

//  Class for any rectangle.
//  D ----- C
//  |       |
//  A ----- B
//  Is represented as 4 points to preserve the shape even after rotation.
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

  // Draw 4 lines using temporary Line objects.
  void draw_svg(std::string &svg) const override;
  // Draw 4 lines using temporary Line objects.
  void draw_pgm(std::vector<std::vector<int>> &pixels) const override;

  // CONSTRUCTORS
public:
  // Create a rectangle with its upper left corner being (x,y), the width w and
  // height h. Must have h,w > 0.
  Rectangle(int x, int y, int w, int h) {
    if (w <= 0) {
      throw std::runtime_error("TODO: rect must have >0 w");
    }
    if (h <= 0) {
      throw std::runtime_error("TODO: rect must have >0 h");
    }

    // Console/Image standard coordinates.
    // (0,0) -- x
    // |
    // y
    A_.set(x, y);
    B_.set(x + w, y);
    C_.set(x + w, y + h);
    D_.set(x, y + h);
  }
};
