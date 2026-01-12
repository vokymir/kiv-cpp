#pragma once

class Point {
private:
  int x_;
  int y_;

public:
  int x() const { return x_; }
  void x(int x) { x_ = x; }

  int y() const { return y_; }
  void y(int y) { y_ = y; }

  void set(int x, int y) {
    x_ = x;
    y_ = y;
  }

  Point() {}
  Point(int x, int y) : x_(x), y_(y) {}

  bool operator==(const Point &other) const {
    return x_ == other.x_ && y_ == other.y_;
  }
};
