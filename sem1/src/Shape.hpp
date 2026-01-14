#pragma once

#include <string>
#include <vector>

// Abstract class. Any shape must be able to transform and draw itself.
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

  // Draw itself in SVG format.
  virtual void draw_svg(std::string &svg) const = 0;
  // Draw itself in PGM format.
  virtual void draw_pgm(std::vector<std::vector<int>> &pixels) const = 0;

  // Allow children have different constructors.
  virtual ~Shape() = default;
};
