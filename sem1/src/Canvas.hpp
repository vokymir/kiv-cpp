#pragma once

#include "Shape.hpp"
#include <memory>
#include <string>
#include <vector>

// Canvas to which any Shape could be added.
// If any transformation is performed, its effects are applied only to currently
// existing Shapes. Newly added Shapes will be added into new "coordinates
// system". Imagine it as if there is a transparent paper; if a transformation
// is performed, the paper is moved/rotated/scaled and new paper is put on top
// of it in the original place. Newly added shapes are put onto the new paper.
// If another transformation is performed, you do it with all the papers in the
// stack.
class Canvas {
private:
  int width_;
  int height_;
  std::vector<std::unique_ptr<Shape>> shapes_;

public:
  // Create new canvas with the dimensions of any given width and height.
  // width and height are not-zero positive integers
  Canvas(int width, int height);

  // TRANSFORMATIONS

  // Move the canvas for x and y.
  // x and y are not constrained integer.
  void translate(int x, int y);
  // Rotate the canvas with the center of rotation (x,y) for angle a.
  // x and y are not constrained integer.
  // a is not constrained angle in degree
  void rotate(int x, int y, float a);
  // Scale the canvas with the center of scaling (x,y) for factor f.
  // x and y are not constrained integer.
  // f is not-zero scale factor
  void scale(int x, int y, float f);

  // SHAPES

  // Add any shape to the canvas. The shape is added to the default coordinate
  // space, ie. the position (0,0) is the same (for adding shape) no matter how
  // many transformations were performed.
  void add_shape(std::unique_ptr<Shape> s);

  // DRAW

  // Return valid SVG string which contains all shapes currently stored.
  std::string draw_svg() const;
  // Return valid PGM string which contains all shapes currently stored.
  std::string draw_pgm() const;
};
