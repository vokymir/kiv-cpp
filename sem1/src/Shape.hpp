#pragma once

#include <string>
#include <vector>

class Shape {
public:
  // Apply translation for x and y.
  // x and y are not constrained integer.
  virtual void translate(int x, int y) = 0;
  // Rotate the shape with the center of rotation (x,y) for angle a.
  // x and y are not constrained integer.
  // a is not constrained angle in degree
  virtual void rotate(int x, int y, float a) = 0;
  // Scale the shape with the center of scaling (x,y) for factor f.
  // x and y are not constrained integer.
  // f is not-zero scale factor
  virtual void scale(int x, int y, float f) = 0;

  virtual std::string draw_svg() const = 0;
  virtual void draw_pgm(std::vector<std::vector<int>> &pixels) const = 0;

  // base-class stuff
  virtual ~Shape() = default;
};
