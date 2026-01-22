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

// ensuring MPInt number has valid precision parameter -> integer >= 4 or
// specific constant for unlimited precision
template <std::size_t PRECISION>
concept Valid_MPInt_Precision = (PRECISION >= 4) || PRECISION == Unlimited;

// how digits are stored inside mpint
using digits_type = std::vector<std::uint8_t>;

// Stores the MPInt_Value, can only create and normalize - all fancy operations
// are implemented on MPInt default value = +0
struct MPInt_Value {
  // ===== Variables =====

  // digits_ is effectivelly an array of unsigned bytes,
  // the LSB is at bytes_[0]
  digits_type digits_{0};

  bool is_positive_{true};

  // ===== Public methods =====

  // make the digits_ vector as small as possible, ensure normal form of value
  void normalize() noexcept {
    // don't allow trailing zeros: 07 > 7, -01 > -1, 00 > 0
    while (digits_.size() > 1 && digits_.back() == 0) {
      digits_.pop_back();
    }

    // only allow one zero, the positive one: -0 > +0
    if (digits_.size() == 1 && digits_.front() == 0) {
      is_positive_ = true;
    }
  }

  // ===== Static constructors =====

  // make mpint_value from any integral type (int, long, ...)
  // returns normalized value
  template <std::integral T> static MPInt_Value from_integral(T number) {
    MPInt_Value value;

    using Unsigned_T = std::make_unsigned_t<T>;
    Unsigned_T u{0};

    // get sign & absolute value
    if constexpr (std::is_signed_v<T>) {
      if (number < 0) { // negative number
        value.is_positive_ = false;

        // handle INT_MIN (in absolute is one bigger than INT_MAX)
        u = static_cast<Unsigned_T>(-(number + 1));
        ++u;

      } else { // positive number
        u = static_cast<Unsigned_T>(number);
      }
    } else { // unsigned number
      u = number;
    }

    // default value is 0, must be definitely cleared before copying digits
    value.digits_.clear();

    // copy digits - do/while because we want to copy 0, which escapes condition
    // on first check
    do {
      value.digits_.push_back(static_cast<std::uint8_t>(u & 0xFF));
      u >>= 8;
    } while (u > 0);

    value.normalize();
    return value;
  }

  // make mpint_value from numeric string - may start with sign +/-, all other
  // characters must be numeric
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

    for (; pos < s.size(); ++pos) { // for each char in str
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

        // only use the lesser 8 bits
        value.digits_[i] = static_cast<std::uint8_t>(tmp & 0xFF);
        // if anything left in tmp, store that in carry
        carry = tmp >> 8;
      }

      // store everything left in carry
      while (carry > 0) {
        value.digits_.push_back(static_cast<std::uint8_t>(carry & 0xFF));
        carry >>= 8;
      }
    }

    value.normalize();
    return value;
  }

  // ===== Static mathematical operations =====

  // === SIGNED ===

  // add two MPInt_Values - signed operation.
  // return new MPInt_Value
  static MPInt_Value add(const MPInt_Value &a, const MPInt_Value &b) {
    MPInt_Value result;

    if (a.is_positive_ == b.is_positive_) { // same sign
      result = add_abs(a, b);               // simply add digits & keep sign
      result.is_positive_ = a.is_positive_;

    } else { // different sign
             // subtract from bigger & keep biggers sign
      int cmp = cmp_abs(a, b);
      const auto &bigger_abs = (cmp >= 0) ? a : b;
      const auto &smaller_abs = (cmp >= 0) ? b : a;

      result = sub_abs(bigger_abs, smaller_abs);
      result.is_positive_ = bigger_abs.is_positive_;
    }

    return result;
  }

  // subtract b from a (signed operation)
  // return new MPInt_Value
  static MPInt_Value sub(const MPInt_Value &a, const MPInt_Value &b) {
    MPInt_Value result;
    int cmp = cmp_abs(a, b);

    if (a.is_positive_ == b.is_positive_) { // same sign
      result = sub_abs(a, b);
      // if b > a: invert sign of a, else keep sign of a
      result.is_positive_ = (cmp < 0) ? !a.is_positive_ : a.is_positive_;

    } else { // different sign
      result = add_abs(a, b);
      // keep sign of a
      result.is_positive_ = a.is_positive_;
    }

    return result;
  }

  // multiply two signed values
  static MPInt_Value mul(const MPInt_Value &a, const MPInt_Value &b) {
    MPInt_Value result = mul_abs(a, b);
    // ++ = +, -- = + | +- = -, -+ = -
    result.is_positive_ = (a.is_positive_ == b.is_positive_);

    return result;
  }

  // divide a by b (signed operation)
  static MPInt_Value div(const MPInt_Value &a, const MPInt_Value &b) {
    MPInt_Value result = div_abs(a, b);
    // same logic as in mul
    result.is_positive_ = (a.is_positive_ == b.is_positive_);

    return result;
  }

  // factorial of a
  // return new MPInt_Value
  static MPInt_Value fct(const MPInt_Value &a) {
    if (!a.is_positive_) {
      throw std::domain_error(
          "Factorial is here only defined on non-negative integers.");
    }

    // start with 1 for 0!
    MPInt_Value result = MPInt_Value::from_integral(1);
    // helper to avoid repeated allocation
    MPInt_Value one = MPInt_Value::from_integral(1);

    // iterate from a to 0, how to read:
    // for (i = a; i > 0; --i)
    for (MPInt_Value i{a}; !i.is_zero(); i = MPInt_Value::sub(i, one)) {
      result = MPInt_Value::mul(result, i);
    }

    return result;
  }

  // === UNSIGNED ===

  static MPInt_Value add_abs(const MPInt_Value &a, const MPInt_Value &b) {}

  static MPInt_Value sub_abs(const MPInt_Value &a, const MPInt_Value &b) {}

  static MPInt_Value mul_abs(const MPInt_Value &a, const MPInt_Value &b) {}

  static MPInt_Value div_abs(const MPInt_Value &a, const MPInt_Value &b) {}

  // ==== Compare operations ====

  // compare the absolute values.
  // return 1 if a > b, -1 if a < b, 0 if a = b
  static int cmp_abs(const MPInt_Value &a, const MPInt_Value &b) {
    // one have more digits
    if (a.digits_.size() != b.digits_.size()) {
      return (a.digits_.size() > b.digits_.size()) ? 1 : -1;
    }

    // from the MSD (most right) traverse all digits
    for (size_t i = a.digits_.size() - 1; i >= 0; --i) {
      if (a.digits_[i] == b.digits_[i]) { // same digit
        continue;
      }

      // different digit
      return (a.digits_[i] > b.digits_[i]) ? 1 : -1;
    }

    // all digits are the same
    return 0;
  }

  // check if MPInt_Value is zero
  bool is_zero() const { return digits_.size() == 1 && digits_[0] == 0; }
};

// forward declare - because the Overflow_Error class needs to know about
// MPInt
template <std::size_t PRECISION>
  requires Valid_MPInt_Precision<PRECISION>
class MPInt;

// specific error for overflowed number. stores the value which caused
// overflow. the stored value is in unlimited precision, so no overflow would
// be caused from it
class Overflow_Error : public std::runtime_error {
  static constexpr std::string default_message = "MPInt overflow";

protected:
  MPInt_Value value_;

public:
  // move constructor
  explicit Overflow_Error(MPInt_Value &&value)
      : std::runtime_error(default_message), value_(std::move(value)) {}

  // copy constructor
  explicit Overflow_Error(const MPInt_Value &value)
      : std::runtime_error(default_message), value_(value) {}

  // move constructor with custom message
  explicit Overflow_Error(MPInt_Value &&value, std::string &&msg)
      : std::runtime_error(std::move(msg)), value_(std::move(value)) {}

  // copy constructor with custom message
  explicit Overflow_Error(const MPInt_Value &value, std::string &&msg)
      : std::runtime_error(std::move(msg)), value_(value) {}

  // get value which caused overflow in unlimited precision
  // (is inline not to viole ODR)
  inline MPInt<Unlimited> unlimited_value() const;
};

// represents any integer number in precision not defined by machine, but by
// parameter PRECISION (in bytes). use any integer >= 4 or MPInt::Unlimited
// for unlimited precision
template <std::size_t PRECISION>
  requires Valid_MPInt_Precision<PRECISION>
class MPInt {
  // debug output for MPInt precision parameter
  static_assert(Valid_MPInt_Precision<PRECISION>,
                "MPInt precision must be >= 4 bytes or MPInt::Unlimited");

  // ===== Variables =====
private:
  // internal storage of number
  MPInt_Value value_;

  // ===== Constuctors =====
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

  // based on given string (must be a valid numeric string with optional
  // leading sign, no whitespaces, no trailing spaces), initialize the MPInt
  // number
  explicit MPInt(const std::string &number_s) {
    auto val = MPInt_Value::from_string(number_s);
    check_overflow(val);
    value_ = std::move(val);
  }

  // wrap the plain value with capable class
  MPInt(const MPInt_Value &value) { value_ = value; }
  MPInt(MPInt_Value &&value) { value_ = std::move(value); }

  // copy/move semantics
  MPInt(const MPInt &) = default;
  MPInt(MPInt &&) noexcept = default;

  // ===== Private methods =====
private:
  // if value is overflown throw error
  static void check_overflow(const MPInt_Value &value) {
    if constexpr (PRECISION == Unlimited) {
      return;
    }

    if (value.digits_.size() > PRECISION) {
      throw Overflow_Error(value);
    }
  }

  // ===== Operator overloads =====
public:
};

// was declared in Overflow_Error, but due to it unknowing the templated class
// MPInt, it must be defined after the MPInt class definition
inline MPInt<Unlimited> Overflow_Error::unlimited_value() const {
  // use constructor from MPInt to construct MPInt from MPInt_Value
  return MPInt<Unlimited>(value_);
}

} // namespace MPInt
