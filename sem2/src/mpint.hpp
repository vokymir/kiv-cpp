#pragma once

#include <concepts>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
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

using digits_type = std::vector<std::uint8_t>;

// bytes_ is effectivelly an array of unsigned bytes,
// the LSB is at bytes_[0]
struct MPInt_Value {
  digits_type digits_;
  bool is_positive_;
};

// forward declare
template <std::size_t PRECISION>
  requires Valid_MPInt_Precision<PRECISION>
class MPInt;

// specific error for overflowed number. find the precise number as public
// attribute of error: value
class Overflow_Error : public std::runtime_error {
protected:
  MPInt_Value value_;

public:
  explicit Overflow_Error(MPInt_Value &&value)
      : std::runtime_error("MPInt overflow"), value_(std::move(value)) {}

  explicit Overflow_Error(const MPInt_Value &value)
      : std::runtime_error("MPInt overflow"), value_(value) {}

  explicit Overflow_Error(MPInt_Value &&value, std::string &&msg)
      : std::runtime_error(std::move(msg)), value_(std::move(value)) {}

  explicit Overflow_Error(const MPInt_Value &value, std::string &&msg)
      : std::runtime_error(std::move(msg)), value_(value) {}

  inline MPInt<Unlimited> unlimited_value() const;
};

template <std::size_t PRECISION>
  requires Valid_MPInt_Precision<PRECISION>
class MPInt {
  // debug output for MPInt precision parameter
  static_assert(Valid_MPInt_Precision<PRECISION>,
                "MPInt precision must be >= 4 bytes or MPInt::Unlimited");

private:
  // internal storage of number
  MPInt_Value value_;

public:
  ~MPInt() = default;
  // let the default value be 0 if none is assigned on creation
  MPInt() : MPInt(0) {}

  // based on given initial number, initialize the MPInt number.
  template <std::integral T> MPInt(T number) {} // TODO:

  // based on given string (must be a valid numeric string with optional leading
  // sign, no whitespaces, no trailing spaces), initialize the MPInt number
  MPInt(const std::string &number_s) {} // TODO:

  // wrap the value with class with all the functionality
  MPInt(const MPInt_Value &value) { value_ = value; }

  // copy semantics
  MPInt(const MPInt &) = default;
  // move semantics
  MPInt(MPInt &&) noexcept = default;

private:
  // Make vector of uint8_t from any integral value. Discards sign. Returns
  // normalized number.
  // TODO: decompose, maybe change based on MPInt_Value
  template <std::integral T> static MPInt_Value make_from_integral(T number) {
    MPInt_Value value;

    // The same type as T, but unsigned.
    using Unsigned_T = std::make_unsigned_t<T>;

    Unsigned_T u;

    // get rid of sign, set u as unsigned value
    value.is_positive_ = true;
    if constexpr (std::is_signed_v<T>) {
      if (number < 0) {
        value.is_positive_ = false;

        // abs(INT_MIN) = abs(INT_MAX) + 1
        // [see](https://www.geeksforgeeks.org/cpp/int_max-int_min-cc-applications/)
        u = static_cast<Unsigned_T>(-(number + 1));
        ++u;
      } else {
        u = static_cast<Unsigned_T>(number);
      }

    } else { // unsigned value is already done
      u = number;
    }

    if (u == 0) {
      value.digits_.push_back(0);
    }

    // copy all bytes from u to tmp
    while (u > 0) {
      // only look at the last 8 bits
      value.digits_.push_back(static_cast<uint8_t>(u & 0xFF));
      u >>= 8;
    }

    normalize(value);
    return value;
  }

  // remove all trailing zeros from number, eg. 05 => 5; 00 => 0
  static void normalize(MPInt_Value &value) {
    while (value.digits_.size() > 1 && value.digits_.back() == 0) {
      value.digits_.pop_back();
    }
  }

  // if number is overflown throw error
  static void check_overflow(const MPInt_Value &value) {
    if constexpr (PRECISION == Unlimited) {
      return;
    }
    if (value.digits_.size() > PRECISION) {
      throw Overflow_Error(value, "overflow error: unknown sign");
    }
  }

  // given vector of bytes, normalize them, check overflow and move them to
  // bytes_
  void set_digits(MPInt_Value &&value) {
    normalize(value);
    check_overflow(value);
    value_ = std::move(value);
  }
};

inline MPInt<Unlimited> Overflow_Error::unlimited_value() const {
  return MPInt<Unlimited>(value_);
}

} // namespace MPInt
