#pragma once

#include <string>

class Shape {
public:
  // Apply translation for x and y.
  // x and y are not constrained integer.
  virtual void translate(int x, int y);
  // Rotate the shape with the center of rotation (x,y) for angle a.
  // x and y are not constrained integer.
  // a is not constrained angle in degree
  virtual void rotate(int x, int y, float a);
  // Scale the shape with the center of scaling (x,y) for factor f.
  // x and y are not constrained integer.
  // f is not-zero scale factor
  virtual void scale(int x, int y, float f);

  virtual std::string draw_svg();
  virtual std::string draw_pgm();
};
