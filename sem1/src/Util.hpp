#pragma once

#include <cmath>
class Util {
public:
  static bool is_zero(float f) {
    const float epsilon = 1e-6;
    return fabsf(f) < epsilon;
  }
};
