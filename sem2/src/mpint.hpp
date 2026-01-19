
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace MPInt {

// static size used for denoting that MPInt number uses *unlimited precision*
static constexpr std::size_t Unlimited = 0;

// concept ensuring MPInt number has valid precision parameter
template <std::size_t N>
concept Valid_MPInt_Length = (N >= 4) || N == MPInt::Unlimited;

template <std::size_t PRECISION>
  requires Valid_MPInt_Length<PRECISION>
class MPInt {
  // debug output for MPInt precision parameter
  static_assert(Valid_MPInt_Length<PRECISION>,
                "MPInt precision must be >= 4 bytes or MPInt::Unlimited");

private:
  // internal storage of number
  std::vector<std::uint8_t> bytes_;

public:
  // let the default value be 0 if none is assigned on creation
  MPInt() : MPInt(0) {}

  // based on given initial number, initialize the MPInt number.
  template <std::integral T> MPInt(T value) {
    if constexpr (PRECISION == Unlimited) {
      bytes_.reserve(4096);
    } else {
      bytes_.resize(PRECISION);
    }
    // TODO: this is definitely not it
  }

  // move, copy
  MPInt(const MPInt &) = default;
  MPInt(MPInt &&) noexcept = default;
};

} // namespace MPInt
