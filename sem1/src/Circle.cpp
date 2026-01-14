
#include "Circle.hpp"
#include <format>
#include <string>
#include <vector>

void Circle::translate(int x, int y) { center_.translate(x, y); }

void Circle::rotate(int x, int y, float a) { center_.rotate(x, y, a); }

void Circle::scale(int x, int y, float f) {
  center_.scale(x, y, f);
  // don't have to check f==0 -> that must have been done in scale
  radius_ *= f;
}

std::string Circle::draw_svg() const {
  return std::format(
      R"(<circle cx="{}" cy="{}" r="{}" stroke="black" stroke-width="2" fill="none" />
)",
      center_.x(), center_.y(), radius_);
}

// midpoint circle
void Circle::draw_pgm(std::vector<std::vector<int>> &pixels) const {
  int cx = center_.x();
  int cy = center_.y();
  int r = radius_;
  int width = pixels[0].size();
  int height = pixels.size();

  int x = r;
  int y = 0;
  int err = 0;

  while (x >= y) {
    auto plot = [&pixels, height, width](int px, int py) {
      if (py >= 0 && py < height && px >= 0 && px < width)
        pixels[py][px] = 0; // black
    };

    // 8way symmetry
    plot(cx + x, cy + y);
    plot(cx + x, cy - y);

    plot(cx + y, cy + x);
    plot(cx + y, cy - x);

    plot(cx - x, cy + y);
    plot(cx - x, cy - y);

    plot(cx - y, cy + x);
    plot(cx - y, cy - x);

    y += 1;
    if (err <= 0) {
      err += 2 * y + 1;
    }
    if (err > 0) {
      x -= 1;
      err -= 2 * x + 1;
    }
  }
}
