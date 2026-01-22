#pragma once

#include "mpint.hpp"
#include <cstddef>

namespace MPTerm {

template <std::size_t P>
  requires(P >= 4 || P == MPInt::Unlimited)
class MPTerm {};

} // namespace MPTerm
