#pragma once

#include <algorithm>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <ostream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

namespace MPInt {

// static size used for denoting that MPInt number uses *unlimited precision*
static constexpr std::size_t Unlimited = 0;

// in this namespace are all the details of implementation, which are not to be
// shown in public API of MPInt:: *module*
// although it is technically possible to use from the outside, it's clear that
// it is not intended
namespace _detail {

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

  // ===== constructors =====

  MPInt_Value() = default;
  // copy/move semantics
  MPInt_Value(const MPInt_Value &) = default;
  MPInt_Value(MPInt_Value &&) noexcept = default;
  MPInt_Value &operator=(const MPInt_Value &) = default;
  MPInt_Value &operator=(MPInt_Value &&) noexcept = default;

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

    // avoiding undefined behaviour on std::uint8_t >> 8
    std::uintmax_t uu = u;

    // copy digits - do/while because we want to copy 0, which escapes condition
    // on first check
    do {
      value.digits_.push_back(static_cast<std::uint8_t>(uu & 0xFF));
      uu >>= 8;
    } while (uu > 0);

    value.normalize();
    return value;
  }

  // make mpint_value from numeric string - may start with sign +/-, all other
  // characters must be numeric
  // assumptions: the numeric literals 0-9 are stored in ascending order list in
  // character representation
  // string is in decadic form
  static MPInt_Value from_string(const std::string_view &s) {
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
  // return normalized MPInt_Value
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

    result.normalize();
    return result;
  }

  // subtract b from a (signed operation)
  // return normalized MPInt_Value
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

    result.normalize();
    return result;
  }

  // multiply two signed values
  // return normalized product
  static MPInt_Value mul(const MPInt_Value &a, const MPInt_Value &b) {
    MPInt_Value result = mul_abs(a, b);
    // ++ = +, -- = + | +- = -, -+ = -
    result.is_positive_ = (a.is_positive_ == b.is_positive_);

    result.normalize();
    return result;
  }

  // divide a by b (signed operation)
  // return normalized quotient
  static MPInt_Value div(const MPInt_Value &a, const MPInt_Value &b) {
    MPInt_Value result = div_abs(a, b);
    // same logic as in mul
    result.is_positive_ = (a.is_positive_ == b.is_positive_);

    result.normalize();
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

    result.normalize();
    return result;
  }

  // === UNSIGNED ===

  // add absolute values of two numbers, ignores sign completely
  // return NOT-normalized MPInt_Value
  static MPInt_Value add_abs(const MPInt_Value &a, const MPInt_Value &b) {
    MPInt_Value result;

    const std::size_t max_size = std::max(a.digits_.size(), b.digits_.size());
    result.digits_.resize(max_size, 0);

    std::uint16_t carry = 0;

    // from LSB add a+b+carry
    for (std::size_t i = 0; i < max_size; ++i) {
      std::uint16_t sum = carry;
      if (i < a.digits_.size()) {
        sum += a.digits_[i];
      }
      if (i < b.digits_.size()) {
        sum += b.digits_[i];
      }

      // only use lower 8 bits, upper 8 is stored in carry
      result.digits_[i] = static_cast<std::uint8_t>(sum & 0xFF);
      carry = sum >> 8;
    }

    // overflow from max_size (don't have to be a problem due to normalization)
    if (carry != 0) {
      result.digits_.push_back(static_cast<std::uint8_t>(carry));
    }

    return result;
  }

  // subtract the absolute value of b from abs(a). if a < b throw.
  // return NOT-normalized MPInt_Value result
  static MPInt_Value sub_abs(const MPInt_Value &a, const MPInt_Value &b) {
    if (MPInt_Value::cmp_abs(a, b) < 0) {
      throw std::domain_error("MPInt_Value::sub_abs(a,b): absolute subtraction "
                              "for a-b require a>b");
    }

    MPInt_Value result;
    result.digits_.resize(a.digits_.size(), 0); // a >= b

    std::int16_t borrow = 0;

    // from LSB to far right
    for (std::size_t i = 0; i < a.digits_.size(); ++i) {
      // subtract: a - b - borrow
      std::int16_t diff = static_cast<std::int16_t>(a.digits_[i]) - borrow;
      if (i < b.digits_.size()) {
        diff -= b.digits_[i];
      }

      // wrap-around in base256 (byte, 2^8)
      if (diff < 0) {
        diff += 256;
        borrow = 1;
      } else {
        borrow = 0;
      }

      // now it's safe to cast diff
      result.digits_[i] = static_cast<std::uint8_t>(diff);
    }

    // borrow will be 0, because a >= b

    return result;
  }

  static MPInt_Value mul_abs(const MPInt_Value &a, const MPInt_Value &b) {
    MPInt_Value result;
    result.digits_.resize(a.digits_.size() + b.digits_.size(), 0);

    for (std::size_t i = 0; i < a.digits_.size(); ++i) {
      std::uint16_t carry = 0;

      for (std::size_t j = 0; j < b.digits_.size(); ++j) {
        std::uint16_t sum =
            static_cast<std::uint16_t>(result.digits_[i + j]) +
            static_cast<std::uint16_t>(a.digits_[i]) * b.digits_[j] + carry;

        result.digits_[i + j] = static_cast<std::uint8_t>(sum & 0xFF);
        carry = sum >> 8;
      }

      // remaining carry - might be loong
      std::size_t k = i + b.digits_.size();
      while (carry > 0) {
        std::uint16_t sum =
            static_cast<std::uint16_t>(result.digits_[k]) + carry;

        result.digits_[k] = static_cast<std::uint8_t>(sum & 0xFF);

        carry = sum >> 8;
        ++k;
      }
    }

    return result;
  }

  static MPInt_Value div_abs(const MPInt_Value &a, const MPInt_Value &b) {
    if (b.is_zero()) {
      throw std::domain_error("MPInt_Value::div_abs: division by zero");
    }

    // a/b && a < b => a/b = 0
    if (cmp_abs(a, b) < 0) {
      return MPInt_Value::from_integral(0);
    }

    // a, but reversed
    MPInt_Value a_rev = a;
    // O(n) but only twice - here and at the end
    std::reverse(a_rev.digits_.begin(), a_rev.digits_.end());

    MPInt_Value quotient;
    quotient.digits_.resize(a.digits_.size(), 0);

    MPInt_Value remainder;
    remainder.digits_.clear();

    // for all stored digits in a_reversed
    for (std::size_t i = 0; i < a_rev.digits_.size(); ++i) {
      remainder.digits_.push_back(a_rev.digits_[i]);

      quotient.digits_[i] = find_quotient_digit(remainder, b);
      remainder =
          sub_abs(remainder,
                  mul_abs(b, MPInt_Value::from_integral(quotient.digits_[i])));
    }

    std::reverse(a_rev.digits_.begin(), a_rev.digits_.end());
    quotient.normalize();
    return quotient;
  }

  // == helper for division ==

  // find the largest single-byte multiplier which satisfies:
  // b * multiplier <= remainder
  static std::uint8_t find_quotient_digit(const MPInt_Value &remainder,
                                          const MPInt_Value &b) {
    std::uint8_t multiplier = 0;
    std::uint8_t low = 0, high = 0xFF;

    while (low <= high) {
      std::uint8_t mid = static_cast<std::uint8_t>((low + high) / 2);
      MPInt_Value trial = mul_abs(b, MPInt_Value::from_integral(mid));

      if (cmp_abs(trial, remainder) <= 0) {
        multiplier = mid;
        low = mid + 1;
      } else {
        high = mid - 1;
      }
    }

    return multiplier;
  }

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

  // ===== when needed to stringify =====

  // divide the value by small divisor (8bits), faster/cheaper but only useful
  // with small numbers - which is when converting to different bases (base10,
  // base16,...)
  //
  // divide val IN_PLACE! and return remainder
  static std::uint8_t div_small(MPInt_Value &val, std::uint8_t div) {
    std::uint16_t carry = 0;
    auto &d = val.digits_;

    // must go from MSD (right-most byte)
    for (auto it = d.rbegin(); it != d.rend(); ++it) {
      std::uint16_t curr = (carry << 8) | *it;
      *it = static_cast<std::uint8_t>(curr / div);
      carry = curr % div;
    }

    val.normalize();
    return static_cast<std::uint8_t>(carry);
  }

  // convert number to string of any base from 2 to 255
  std::string to_string(std::uint8_t base = 10) const {
    if (digits_.size() == 1 && digits_[0] == 0) {
      return "0";
    }

    if (base < 2 || base > 255) {
      throw std::domain_error("Not supported base for number conversion.");
    }

    MPInt_Value tmp{*this}; // copy this for in-place division
    std::string out; // initially stored in reversed order, fixed at the end

    // divide by base and store remainder
    while (!(tmp.digits_.size() == 1 && tmp.digits_[0] == 0)) {
      auto rem = div_small(tmp, base);
      out.push_back(static_cast<char>('0' + rem));
    }

    if (!is_positive_) {
      out.push_back('-');
    }

    std::reverse(out.begin(), out.end());
    return out;
  }
};

} // namespace _detail

template <std::size_t PRECISION>
  requires _detail::Valid_MPInt_Precision<PRECISION>
class MPInt;

// MPInt was here only forward declared - because the Overflow_Error class needs
// to know about MPInt

// specific error for overflowed number. stores the value which caused
// overflow. the stored value is in unlimited precision, so no overflow would
// be caused from it
class Overflow_Error : public std::runtime_error {
  static constexpr std::string default_message = "MPInt overflow";

protected:
  _detail::MPInt_Value value_;

public:
  // move constructor
  explicit Overflow_Error(_detail::MPInt_Value &&value)
      : std::runtime_error(default_message), value_(std::move(value)) {}

  // copy constructor
  explicit Overflow_Error(const _detail::MPInt_Value &value)
      : std::runtime_error(default_message), value_(value) {}

  // move constructor with custom message
  explicit Overflow_Error(_detail::MPInt_Value &&value, std::string &&msg)
      : std::runtime_error(std::move(msg)), value_(std::move(value)) {}

  // copy constructor with custom message
  explicit Overflow_Error(const _detail::MPInt_Value &value, std::string &&msg)
      : std::runtime_error(std::move(msg)), value_(value) {}

  // get value which caused overflow in unlimited precision
  // (is inline not to viole ODR)
  inline MPInt<Unlimited> unlimited_value() const;
};

// represents any integer number in precision not defined by machine, but by
// parameter PRECISION (in bytes). use any integer >= 4 or MPInt::Unlimited
// for unlimited precision
template <std::size_t PRECISION>
  requires _detail::Valid_MPInt_Precision<PRECISION>
class MPInt {
  // debug output for MPInt precision parameter
  static_assert(_detail::Valid_MPInt_Precision<PRECISION>,
                "MPInt precision must be >= 4 bytes or MPInt::Unlimited");

  // ===== Variables =====
private:
  // internal storage of number
  _detail::MPInt_Value value_;

  // ===== Constuctors =====
public:
  ~MPInt() = default;

  // let the default value be whatever is in MPInt_Value
  // (There is sane default: 0)
  MPInt() {}

  // based on given initial number, initialize the MPInt number.
  template <std::integral T> explicit MPInt(T number) {
    auto val = _detail::MPInt_Value::from_integral(number);
    check_overflow(val);
    value_ = std::move(val);
  }

  // based on given string (must be a valid numeric string with optional
  // leading sign, no whitespaces, no trailing spaces), initialize the MPInt
  // number
  explicit MPInt(const std::string_view &number_s) {
    auto val = _detail::MPInt_Value::from_string(number_s);
    check_overflow(val);
    value_ = std::move(val);
  }

  // wrap the plain value with capable class
  MPInt(const _detail::MPInt_Value &value) { value_ = value; }
  MPInt(_detail::MPInt_Value &&value) { value_ = std::move(value); }

  // copy/move semantics
  MPInt(const MPInt &) = default;
  MPInt(MPInt &&) noexcept = default;
  MPInt &operator=(const MPInt &) = default;
  MPInt &operator=(MPInt &&) noexcept = default;

  // ===== Private methods =====
private:
  // if value is overflown throw error
  static void check_overflow(const _detail::MPInt_Value &value) {
    if constexpr (PRECISION == Unlimited) {
      return;
    }

    if (value.digits_.size() > PRECISION) {
      throw Overflow_Error(value);
    }
  }

  // ===== Operator overloads =====
public:
  // compound operator for adding: keep precision of this MPInt, may throw on
  // overflow
  template <std::size_t OTHER_PRECISION>
  MPInt<PRECISION> &operator+=(const MPInt<OTHER_PRECISION> &rhs) {
    auto value = _detail::MPInt_Value::add(value_, rhs.value_);
    check_overflow(value);
    value_ = std::move(value);
    return *this;
  }

  // compound operator for subtracting: keep precision of this MPInt, may throw
  // on overflow
  template <std::size_t OTHER_PRECISION>
  MPInt<PRECISION> &operator-=(const MPInt<OTHER_PRECISION> &rhs) {
    auto value = _detail::MPInt_Value::sub(value_, rhs.value_);
    check_overflow(value);
    value_ = std::move(value);
    return *this;
  }

  // compound operator for multiplication: keep precision of this MPInt, may
  // throw on overflow
  template <std::size_t OTHER_PRECISION>
  MPInt<PRECISION> &operator*=(const MPInt<OTHER_PRECISION> &rhs) {
    auto value = _detail::MPInt_Value::mul(value_, rhs.value_);
    check_overflow(value);
    value_ = std::move(value);
    return *this;
  }

  // compound operator for division: keep precision of this MPInt, may throw on
  // overflow
  template <std::size_t OTHER_PRECISION>
  MPInt<PRECISION> &operator/=(const MPInt<OTHER_PRECISION> &rhs) {
    auto value = _detail::MPInt_Value::div(value_, rhs.value_);
    check_overflow(value);
    value_ = std::move(value);
    return *this;
  }

  // helper: decide which precision is higher from given two
  template <std::size_t A, std::size_t B>
  static constexpr std::size_t higher_precision =
      (A == Unlimited || B == Unlimited) ? Unlimited : (A > B ? A : B);

  // operator for adding: keep higher precision, is using += internally
  template <std::size_t OTHER_PRECISION>
  MPInt<higher_precision<PRECISION, OTHER_PRECISION>>
  operator+(const MPInt<OTHER_PRECISION> &other) {
    MPInt<higher_precision<PRECISION, OTHER_PRECISION>> result(*this);
    result += other;
    return result;
  }

  // operator for subtracting: keep higher precision, is using -= internally
  template <std::size_t OTHER_PRECISION>
  MPInt<higher_precision<PRECISION, OTHER_PRECISION>>
  operator-(const MPInt<OTHER_PRECISION> &other) {
    MPInt<higher_precision<PRECISION, OTHER_PRECISION>> result(*this);
    result -= other;
    return result;
  }

  // operator for multiplying: keep higher precision, is using *= internally
  template <std::size_t OTHER_PRECISION>
  MPInt<higher_precision<PRECISION, OTHER_PRECISION>>
  operator*(const MPInt<OTHER_PRECISION> &other) {
    MPInt<higher_precision<PRECISION, OTHER_PRECISION>> result(*this);
    result *= other;
    return result;
  }

  // operator for dividing: keep higher precision, is using /= internally
  template <std::size_t OTHER_PRECISION>
  MPInt<higher_precision<PRECISION, OTHER_PRECISION>>
  operator/(const MPInt<OTHER_PRECISION> &other) {
    MPInt<higher_precision<PRECISION, OTHER_PRECISION>> result(*this);
    result /= other;
    return result;
  }

  // factorial: keep precision
  void factorial() {
    auto value = _detail::MPInt_Value::fct(value_);
    check_overflow(value);
    value_ = std::move(value);
  }

  // convert to decadic string representation
  std::string to_string() const { return value_.to_string(); }
};

// convert number to base10 numeric string
template <std::size_t P>
std::ostream &operator<<(std::ostream &os, const MPInt<P> &number) {
  return os << number.to_string();
}

// was declared in Overflow_Error, but due to it unknowing the templated class
// MPInt, it must be defined after the MPInt class definition
inline MPInt<Unlimited> Overflow_Error::unlimited_value() const {
  // use constructor from MPInt to construct MPInt from MPInt_Value
  return MPInt<Unlimited>(value_);
}

} // namespace MPInt
