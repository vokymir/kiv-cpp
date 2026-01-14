#pragma once

#include <cmath>

// (Potentially) shared functionality.
// If contains more than 3 methods, something is probably wrong.
class Util {
public:
  // Does given float number equal zero?
  // Can specify accuracy, sensible default for most applications.
  static bool is_zero(float f, float epsilon = 1e-6) {
    return fabsf(f) < epsilon;
  }
};
