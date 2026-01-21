
#include <algorithm>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

namespace MPInt {

// static size used for denoting that MPInt number uses *unlimited precision*
static constexpr std::size_t Unlimited = 0;

// concept ensuring MPInt number has valid precision parameter
template <std::size_t PRECISION>
concept Valid_MPInt_Precision = (PRECISION >= 4) || PRECISION == Unlimited;

template <std::size_t PRECISION>
  requires Valid_MPInt_Precision<PRECISION>
class MPInt;

// specific error for overflowed number. find the precise number as public
// attribute of error: value
class Overflow_Error : public std::runtime_error {
  using MPInt_Unlimited = std::unique_ptr<MPInt<Unlimited>>;

public:
  MPInt_Unlimited value;

  explicit Overflow_Error(MPInt_Unlimited v)
      : std::runtime_error("MPInt overflow"), value(std::move(v)) {}

  explicit Overflow_Error(MPInt_Unlimited v, std::string &&msg)
      : std::runtime_error(std::move(msg)), value(std::move(v)) {}
};

template <std::size_t PRECISION>
  requires Valid_MPInt_Precision<PRECISION>
class MPInt {
  // debug output for MPInt precision parameter
  static_assert(Valid_MPInt_Precision<PRECISION>,
                "MPInt precision must be >= 4 bytes or MPInt::Unlimited");

  using digits_ = std::vector<std::uint8_t>;

private:
  // internal storage of number

  // bytes_ is effectivelly an array of unsigned bytes,
  // the LSB is at bytes_[0]
  digits_ bytes_;
  bool is_positive_ = true;

public:
  // let the default value be 0 if none is assigned on creation
  MPInt() : MPInt(0) {}
  virtual ~MPInt() = default;

  // based on given initial number, initialize the MPInt number.
  template <std::integral T> MPInt(T value) {
    if constexpr (PRECISION == Unlimited) {
      bytes_.reserve(4096);
    } else {
      bytes_.reserve(PRECISION);
    }

    // determine sign of given number
    is_positive_ = true;
    if (value < 0) {
      is_positive_ = false;
    }

    // copy number (without sign) to bytes_
    auto tmp = make_from_integral(value);
    set_digits(std::move(tmp));
  }

  MPInt(digits_ &&d, bool sign) : bytes_(std::move(d)), is_positive_(sign) {}
  MPInt(const digits_ &d, bool sign) : bytes_(d), is_positive_(sign) {}

  // move, copy
  MPInt(const MPInt &) = default;
  MPInt(MPInt &&) noexcept = default;

private:
  // Make vector of uint8_t from any integral value. Discards sign. Returns
  // normalized number.
  template <std::integral T> static digits_ make_from_integral(T value) {
    digits_ tmp;

    // The same type as T, but unsigned.
    using Unsigned_T = std::make_unsigned_t<T>;

    Unsigned_T u;

    // get rid of sign, set u as unsigned value
    if constexpr (std::is_signed_v<T>) {
      if (value < 0) {
        // abs(INT_MIN) = abs(INT_MAX) + 1
        // [see](https://www.geeksforgeeks.org/cpp/int_max-int_min-cc-applications/)
        u = static_cast<Unsigned_T>(-(value + 1));
        ++u;
      } else {
        u = static_cast<Unsigned_T>(value);
      }

    } else { // unsigned value is already done
      u = value;
    }

    if (u == 0) {
      tmp.push_back(0);
    }

    // copy all bytes from u to tmp
    while (u > 0) {
      // only look at the last 8 bits
      tmp.push_back(static_cast<uint8_t>(u & 0xFF));
      u >>= 8;
    }

    normalize(tmp);
    return tmp;
  }

  // remove all trailing zeros from number, eg. 05 => 5; 00 => 0
  static void normalize(digits_ &d) {
    while (d.size() > 1 && d.back() == 0) {
      d.pop_back();
    }
  }

  // if number is overflown throw error
  static void check_overflow(const digits_ &d) {
    if constexpr (PRECISION == Unlimited) {
      return;
    }
    if (d.size() > PRECISION) {
      auto full_ptr = std::make_unique<MPInt<Unlimited>>(d, true);
      throw Overflow_Error(std::move(full_ptr), "overflow error: unknown sign");
    }
  }

  // given vector of bytes, normalize them, check overflow and move them to
  // bytes_
  void set_digits(digits_ &&d) {
    normalize(d);
    check_overflow(d);
    bytes_ = std::move(d);
  }
};

} // namespace MPInt
