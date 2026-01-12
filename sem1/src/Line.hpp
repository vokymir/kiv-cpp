#pragma once

#include "Point.hpp"
#include "Shape.hpp"

class Line : public Shape {
private:
  Point p1;
  Point p2;

public:
  void translate(int x, int y) override;
  void rotate(int x, int y, float a) override;
  void scale(int x, int y, float f) override;

  std::string draw_svg() override;
  std::string draw_pgm() override;
};
