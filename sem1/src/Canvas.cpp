#include "Canvas.hpp"
#include "Shape.hpp"
#include <memory>
#include <stdexcept>
#include <string>

Canvas::Canvas(int w, int h) {
  if (w <= 0) {
    throw std::runtime_error("TODO: canvas w must be >0");
  }
  if (h <= 0) {
    throw std::runtime_error("TODO: canvas h must be >0");
  }

  width_ = w;
  height_ = h;
}

void Canvas::translate(int x, int y) {
  for (auto &shape : shapes_) {
    shape->translate(x, y);
  }
}

void Canvas::rotate(int x, int y, float a) {
  for (auto &shape : shapes_) {
    shape->rotate(x, y, a);
  }
}

void Canvas::scale(int x, int y, float f) {
  for (auto &shape : shapes_) {
    shape->scale(x, y, f);
  }
}

void Canvas::add_shape(std::unique_ptr<Shape> s) {
  shapes_.push_back(std::move(s));
}

std::string Canvas::draw_svg() {
  std::string output;

  // init the svg string

  for (const auto &shape : shapes_) {
    output += shape->draw_svg();
  }

  // end the svg string

  return output;
}

std::string Canvas::draw_pgm() {
  std::string output;

  // init the pgm string

  for (const auto &shape : shapes_) {
    output += shape->draw_pgm();
  }

  // end the pgm string

  return output;
}
