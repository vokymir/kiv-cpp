#pragma once

class Point {
private:
  int x_;
  int y_;

public:
  int x() const { return x_; }
  void x(int new_x) { x_ = new_x; }

  int y() const { return y_; }
  void y(int new_y) { y_ = new_y; }

  Point() {}
  Point(int x, int y) : x_(x), y_(y) {}

  bool operator==(const Point &other) const {
    return x_ == other.x_ && y_ == other.y_;
  }
};
