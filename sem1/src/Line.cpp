
#include "Line.hpp"
#include <format>
#include <string>
#include <vector>

void Line::translate(int x, int y) {
  p1_.translate(x, y);
  p2_.translate(x, y);
}

void Line::rotate(int x, int y, float a) {
  p1_.rotate(x, y, a);
  p2_.rotate(x, y, a);
}

void Line::scale(int x, int y, float f) {
  p1_.scale(x, y, f);
  p2_.scale(x, y, f);
}

std::string Line::draw_svg() const {
  return std::format(
      R"(<line x1="{}" y1="{}" x2="{}" y2="{}" stroke="black" stroke-width="2" />
)",
      p1_.x(), p1_.y(), p2_.x(), p2_.y());
}

void Line::draw_pgm(std::vector<std::vector<int>> &pixels) const {
  // Bresenham
  int x0 = p1_.x(), y0 = p1_.y();
  int x1 = p2_.x(), y1 = p2_.y();

  int dx = std::abs(x1 - x0);
  int dy = -std::abs(y1 - y0);
  int sx = x0 < x1 ? 1 : -1;
  int sy = y0 < y1 ? 1 : -1;
  int err = dx + dy;

  while (true) {
    if (y0 >= 0 && y0 < pixels.size() && x0 >= 0 && x0 < pixels[0].size())
      pixels[y0][x0] = 0; // black pixel

    if (x0 == x1 && y0 == y1)
      break;
    int e2 = 2 * err;
    if (e2 >= dy) {
      err += dy;
      x0 += sx;
    }
    if (e2 <= dx) {
      err += dx;
      y0 += sy;
    }
  }
}
