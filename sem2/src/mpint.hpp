
#include <cstddef>

namespace MPInt {

static constexpr std::size_t Unlimited = 0;

template <std::size_t N>
concept Valid_MPInt_Length = (N >= 4) || N == MPInt::Unlimited;

template <std::size_t N>
  requires Valid_MPInt_Length<N>
class MPInt {
  static_assert(Valid_MPInt_Length<N>,
                "MPInt precision must be >= 4 bytes or MPInt::Unlimited");
};

} // namespace MPInt
