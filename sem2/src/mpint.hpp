#pragma once

#include <algorithm>
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
struct MPInt_Struct {
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
  MPInt_Struct value_;

public:
  explicit Overflow_Error(MPInt_Struct &&v)
      : std::runtime_error("MPInt overflow"), value_(std::move(v)) {}

  explicit Overflow_Error(const MPInt_Struct &v)
      : std::runtime_error("MPInt overflow"), value_(v) {}

  explicit Overflow_Error(MPInt_Struct &&v, std::string &&msg)
      : std::runtime_error(std::move(msg)), value_(std::move(v)) {}

  explicit Overflow_Error(const MPInt_Struct &v, std::string &&msg)
      : std::runtime_error(std::move(msg)), value_(v) {}

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
  MPInt_Struct value_;

public:
  // let the default value be 0 if none is assigned on creation
  MPInt() : MPInt(0) {}
  virtual ~MPInt() = default;

  // based on given initial number, initialize the MPInt number.
  template <std::integral T> MPInt(T value) {
    if constexpr (PRECISION == Unlimited) {
      value_.digits_.reserve(4096);
    } else {
      value_.digits_.reserve(PRECISION);
    }

    // determine sign of given number
    value_.is_positive_ = true;
    if (value < 0) {
      value_.is_positive_ = false;
    }

    // copy number (without sign) to bytes_
    auto tmp = make_from_integral(value);
    set_digits(std::move(tmp));
  }

  MPInt(const std::string &value) {
    if (value.empty()) {
      throw std::invalid_argument("Cannot construct MPInt from empty string");
    }

    // determine sign
    value_.is_positive_ = true;
    std::size_t start = 0;
    if (value[0] == '-') {
      value_.is_positive_ = false;
      start = 1;
    } else if (value[0] == '+') {
      start = 1;
    }

    digits_type tmp{0}; // start with 0

    // for all chars inside string
    for (std::size_t i = start; i < value.size(); ++i) {
      if (value[i] < '0' || value[i] > '9') {
        throw std::invalid_argument("Invalid character in MPInt string");
      }
      int digit = value[i] - '0';

      // multiply current tmp by 10 and add digit
      int carry = digit;
      for (std::size_t j = 0; j < tmp.size(); ++j) {
        int val = tmp[j] * 10 + carry;
        tmp[j] = val & 0xFF; // low 8 bits
        carry = val >> 8;    // carry to next byte
      }
      while (carry > 0) {
        tmp.push_back(carry & 0xFF);
        carry >>= 8;
      }
    }

    set_digits(std::move(tmp));
  }

  MPInt(const MPInt_Struct &inner) { value_ = inner; }

  // move, copy
  MPInt(const MPInt &) = default;
  MPInt(MPInt &&) noexcept = default;

private:
  // Make vector of uint8_t from any integral value. Discards sign. Returns
  // normalized number.
  template <std::integral T> static MPInt_Struct make_from_integral(T value) {
    MPInt_Struct res;

    // The same type as T, but unsigned.
    using Unsigned_T = std::make_unsigned_t<T>;

    Unsigned_T u;

    // get rid of sign, set u as unsigned value
    res.is_positive_ = true;
    if constexpr (std::is_signed_v<T>) {
      if (value < 0) {
        res.is_positive_ = false;

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
      res.digits_.push_back(0);
    }

    // copy all bytes from u to tmp
    while (u > 0) {
      // only look at the last 8 bits
      res.digits_.push_back(static_cast<uint8_t>(u & 0xFF));
      u >>= 8;
    }

    normalize(res);
    return res;
  }

  // remove all trailing zeros from number, eg. 05 => 5; 00 => 0
  static void normalize(MPInt_Struct &inner) {
    while (inner.digits_.size() > 1 && inner.digits_.back() == 0) {
      inner.digits_.pop_back();
    }
  }

  // if number is overflown throw error
  static void check_overflow(const MPInt_Struct &inner) {
    if constexpr (PRECISION == Unlimited) {
      return;
    }
    if (inner.digits_.size() > PRECISION) {
      throw Overflow_Error(inner, "overflow error: unknown sign");
    }
  }

  // given vector of bytes, normalize them, check overflow and move them to
  // bytes_
  void set_digits(MPInt_Struct &&inner) {
    normalize(inner);
    check_overflow(inner);
    value_ = std::move(inner);
  }
};

inline MPInt<Unlimited> Overflow_Error::unlimited_value() const {
  return MPInt<Unlimited>(value_);
}

} // namespace MPInt
