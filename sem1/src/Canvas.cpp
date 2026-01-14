#include "Canvas.hpp"
#include "Shape.hpp"
#include <format>
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

std::string Canvas::draw_svg() const {
  // SVG header
  std::string output =
      std::format(R"(<svg width="{}" height="{}" style="background-color:white">
)",
                  width_, height_);

  // draw every shape on canvas
  for (const auto &shape : shapes_) {
    shape->draw_svg(output);
  }

  // SVG footer
  output += "\n</svg>\n";

  return output;
}

std::string Canvas::draw_pgm() const {
  // PGM header
  std::string output = std::format("P2\n{} {}\n255\n", width_, height_);

  // draw every shape on white canvas
  std::vector<std::vector<int>> pixels(height_, std::vector<int>(width_, 255));
  for (const auto &shape : shapes_) {
    shape->draw_pgm(pixels);
  }

  // transform 2D image to string
  for (int y = 0; y < height_; ++y) {
    for (int x = 0; x < width_; ++x) {
      output += std::to_string(pixels[y][x]) + " ";
    }
    output += "\n";
  }

  // NO PGM footer

  return output;
}
