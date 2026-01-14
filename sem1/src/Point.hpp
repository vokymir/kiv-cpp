#pragma once

// Representation of 2D point, defined by x,y coordinates.
// Capable of performing translations on itself.
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

  // Create new point from coordinates.
  Point(int x, int y) : x_(x), y_(y) {}

  Point() : Point(0, 0) {}

  // Are two points equal? Only if both theirs coordinates are equal.
  bool operator==(const Point &other) const {
    return x_ == other.x_ && y_ == other.y_;
  }

  // Move the point by x and y in corresponding axes.
  void translate(int x, int y);

  // Rotate the point for angle a (in degrees) with the origin of rotation being
  // (x,y).
  void rotate(int x, int y, float a);

  // Move the point adequately for the canvas resizing by scale factor f with
  // the origin of scaling being (x,y).
  void scale(int x, int y, float f);
};
