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
struct MPInt_Value {
  // default value = +0
  digits_type digits_{1, 0};
  bool is_positive_{true};

  void normalize() noexcept {
    while (digits_.size() > 1 && digits_.back() == 0) {
      digits_.pop_back();
    }
  }

  template <std::integral T> static MPInt_Value from_integral(T number) {
    MPInt_Value value;

    using Unsigned_T = std::make_unsigned_t<T>;
    Unsigned_T u{0};

    // get sign & absolute value
    if constexpr (std::is_signed_v<T>) {
      if (number < 0) {
        value.is_positive_ = false;

        // handle INT_MIN (in absolute is one bigger than INT_MAX)
        u = static_cast<Unsigned_T>(-(number + 1));
        ++u;
      } else {
        u = static_cast<Unsigned_T>(number);
      }
    } else {
      u = number;
    }

    // copy digits
    while (u > 0) {
      value.digits_.push_back(static_cast<std::uint8_t>(u & 0xFF));
    }

    value.normalize();
    return value;
  }

  // assumptions: the numeric literals 0-9 are stored in ascending order list in
  // character representation
  // string is in decadic form
  static MPInt_Value from_string(const std::string &s) {
    if (s.empty()) {
      throw std::invalid_argument("MPInt_Value: empty string");
    }

    MPInt_Value value;
    std::size_t pos = 0;

    // sign
    if (s[0] == '-') {
      value.is_positive_ = false;
      pos = 1;
    } else if (s[0] == '+') {
      pos = 1;
    }

    if (pos == s.size()) {
      throw std::invalid_argument("MPInt_Value: invalid numeric string");
    }

    for (; pos < s.size(); ++pos) {
      char c = s[pos];

      if (c < '0' || c > '9') {
        throw std::invalid_argument("MPInt_Value: invalid character in string");
      }

      int digit = c - '0';
      std::uint16_t carry = digit;

      // multiply curr value by 10 and add digit
      for (std::size_t i = 0; i < value.digits_.size(); ++i) {
        std::uint16_t tmp =
            static_cast<std::uint16_t>(value.digits_[i]) * 10 + carry;
        value.digits_[i] = static_cast<std::uint8_t>(tmp & 0xFF);
        carry = tmp >> 8;
      }

      while (carry > 0) {
        value.digits_.push_back(static_cast<std::uint8_t>(carry & 0xFF));
        carry >>= 8;
      }
    }

    value.normalize();
    return value;
  }
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

  // let the default value be whatever is in MPInt_Value
  // (There is sane default: 0)
  MPInt() {}

  // based on given initial number, initialize the MPInt number.
  template <std::integral T> explicit MPInt(T number) {
    auto val = MPInt_Value::from_integral(number);
    check_overflow(val);
    value_ = std::move(val);
  }

  // based on given string (must be a valid numeric string with optional leading
  // sign, no whitespaces, no trailing spaces), initialize the MPInt number
  explicit MPInt(const std::string &number_s) {
    auto val = MPInt_Value::from_string(number_s);
    check_overflow(val);
    value_ = std::move(val);
  }

  // wrap the value with class with all the functionality
  MPInt(const MPInt_Value &value) { value_ = value; }
  MPInt(MPInt_Value &&value) { value_ = std::move(value); }

  // copy semantics
  MPInt(const MPInt &) = default;
  // move semantics
  MPInt(MPInt &&) noexcept = default;

private:
  // if number is overflown throw error
  static void check_overflow(const MPInt_Value &value) {
    if constexpr (PRECISION == Unlimited) {
      return;
    }

    if (value.digits_.size() > PRECISION) {
      throw Overflow_Error(value);
    }
  }
};

inline MPInt<Unlimited> Overflow_Error::unlimited_value() const {
  return MPInt<Unlimited>(value_);
}

} // namespace MPInt
