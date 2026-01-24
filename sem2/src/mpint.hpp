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

// Multiple Precision Integer number library. Use the provided MPInt class. All
// implementation details are hidden inside _detail namespace, which is not
// advised to use directly.
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

// how digits are stored inside mpint, being the LSB is at index 0
// Why vector:
// + provides random access
// + append is O(1) - good for most operations
// - prepend is O(n) - bad for division
// NOTE: if at any time this is changed, complete re-thinking of the project
// might be needed
using digits_type = std::vector<std::uint8_t>;

// Struct which in fact represents the multiple precision value. Provides useful
// methods for constructing and supported math operations.
// The *flagship* class MPInt uses this and only add upon it.
//
// Main reason for existence is the Overflow_Error, which needs to be able to
// store MPInt, but is used inside MPInt. So this struct breaks circular
// dependency.
struct MPInt_Value {
  // ===== Variables =====
  // as this is only internally used in MPInt, no private variables and
  // getters/setters were used
public:
  // digits_ is effectivelly an array of unsigned bytes,
  // the LSB is at bytes_[0]
  // default value is 0, it being one byte which equal zero
  digits_type digits_{0};

  // support for negative numbers, while having the simple unsigned number
  // representation in digits_
  bool is_positive_{true};

  // ===== Public methods =====

  // make the digits_ vector as small as possible, ensure normal form of value
  void normalize() noexcept {
    // don't allow trailing zeros: 07 -> 7, -01 -> -1, 00 -> 0
    while (digits_.size() > 1 && digits_.back() == 0) {
      digits_.pop_back();
    }

    // only allow one zero, the positive one: -0 -> +0
    if (digits_.size() == 1 && digits_.front() == 0) {
      is_positive_ = true;
    }
  }

  // ===== Constructors =====

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
      // only use lowest 8 bits
      value.digits_.push_back(static_cast<std::uint8_t>(uu & 0xFF));
      uu >>= 8;
    } while (uu > 0);

    value.normalize();
    return value;
  }

  // make mpint_value from numeric decadic string - may start with sign +/-, all
  // other characters must be numeric. assumptions: the numeric literals 0-9 are
  // stored in ascending order list in character representation; string is in
  // decadic form
  static MPInt_Value from_string(const std::string_view &s) {
    if (s.empty()) {
      throw std::invalid_argument("MPInt_Value: empty string");
    }

    MPInt_Value value;
    std::size_t pos = 0;

    // sign (default is positive)
    if (s[0] == '-') {
      value.is_positive_ = false;
      pos = 1;
    } else if (s[0] == '+') {
      pos = 1;
    }

    if (pos == s.size()) { // only sign
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
      if (cmp >= 0) {
        result = sub_abs(a, b);
      } else {
        result = sub_abs(b, a);
      }
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

  // factorial of a (signed, but only works on a >= 0)
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
    // for (i = a; i != 0; --i)
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

    // prepare bytes in result
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

    // bigger than expected max_size
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
    result.digits_.resize(a.digits_.size(), 0); // a >= b, that's why size of a

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

  // multiply abs(a) * abs(b)
  // doesn't normalize, return new MPInt_Value
  static MPInt_Value mul_abs(const MPInt_Value &a, const MPInt_Value &b) {
    MPInt_Value result;
    // at most will have this many digits
    result.digits_.resize(a.digits_.size() + b.digits_.size(), 0);

    // O(mn), each digit from a with each digit from b
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

  // division of abs(a)/abs(b)
  // return new MPInt_Value which is NOT normalized
  static MPInt_Value div_abs(const MPInt_Value &a, const MPInt_Value &b) {
    if (b.is_zero()) {
      throw std::domain_error("MPInt_Value::div_abs: division by zero");
    }

    // a/b && a < b => a/b = 0
    if (cmp_abs(a, b) < 0) {
      return MPInt_Value::from_integral(0);
    }

    MPInt_Value quotient;
    // at most will have this many digits
    quotient.digits_.resize(a.digits_.size(), 0);

    MPInt_Value remainder;

    for (std::size_t i = a.digits_.size(); i-- > 0;) {

      remainder.normalize(); // for the zero check to work
      // insert at begin is O(n) operation, so avoid if possible
      if (!remainder.is_zero()) {
        // shift remainder by one byte
        remainder.digits_.insert(remainder.digits_.begin(), 0);
      }
      // add byte from a
      remainder.digits_[0] = a.digits_[i];
      remainder.normalize();

      // find how many times b fits in remainder
      std::uint8_t q = find_quotient_digit(remainder, b);
      quotient.digits_[i] = q;

      // subtract b*q from remainder
      if (q != 0) {
        MPInt_Value product = mul(b, MPInt_Value::from_integral(q));
        remainder = sub(remainder, product);
      }
    }

    quotient.normalize();
    return quotient;
  }

  // == Helper for division ==

  // find the largest single-byte multiplier which satisfies:
  // b * multiplier <= remainder
  // using binary search
  static std::uint8_t find_quotient_digit(const MPInt_Value &remainder,
                                          const MPInt_Value &b) {
    int multiplier = 0;
    int low = 0;
    int high = 255;

    while (low <= high) {
      int mid = (low + high) / 2;

      MPInt_Value trial = mul(b, MPInt_Value::from_integral(mid));

      if (cmp_abs(trial, remainder) <= 0) {
        multiplier = mid;
        low = mid + 1;
      } else {
        high = mid - 1;
      }
    }

    return static_cast<std::uint8_t>(multiplier);
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
    for (size_t i = a.digits_.size(); i > 0; --i) {
      if (a.digits_[i - 1] == b.digits_[i - 1]) { // same digit
        continue;
      }

      // different digit
      return (a.digits_[i - 1] > b.digits_[i - 1]) ? 1 : -1;
    }

    // all digits are the same
    return 0;
  }

  // check if MPInt_Value is zero
  bool is_zero() const { return digits_.size() == 1 && digits_[0] == 0; }

  // ===== When need to stringify =====

  // divide the value by small divisor (8bits), faster/cheaper but only useful
  // with small numbers - which is when converting to different bases (base10,
  // base16,...)
  //
  // divide val IN_PLACE! and return remainder
  static std::uint8_t div_small(MPInt_Value &val, std::uint8_t div) {
    std::uint16_t carry = 0;
    auto &d = val.digits_;

    // must go from MSD (right-most byte)
    // works defacto the same as division by-hand
    for (auto it = d.rbegin(); it != d.rend(); ++it) {
      std::uint16_t curr = (carry << 8) | *it;
      *it = static_cast<std::uint8_t>(curr / div);
      carry = curr % div;
    }

    val.normalize();
    return static_cast<std::uint8_t>(carry);
  }

  // convert number to string of any base from 2 to 9+26=35
  // only bases from 2 up makes sense to me. from 10+ it requires letters, which
  // are 26 in the english alphabet, that's where the upper limit comes from -
  // it could be 255 if I figured out the representation
  std::string to_string(std::uint8_t base = 10) const {
    if (digits_.size() == 1 && digits_[0] == 0) {
      return "0";
    }

    if (base < 2 || base > 35) {
      throw std::domain_error(
          "Not supported base for number conversion. Supported are: 2-35");
    }

    MPInt_Value tmp{*this}; // copy this for in-place division
    std::string out; // initially stored in reversed order, fixed at the end

    // divide by base and store remainder
    while (!tmp.is_zero()) {
      auto rem = div_small(tmp, base);
      if (rem < 10) { // bases lesser than 10
        out.push_back('0' + rem);
      } else { // higher bases uses capital letters
        out.push_back('A' + (rem - 10));
      }
    }

    if (!is_positive_) {
      out.push_back('-');
    }

    // we have the string reversed for obvious reasons
    std::reverse(out.begin(), out.end());
    return out;
  }
};

} // namespace _detail

template <std::size_t PRECISION>
  requires _detail::Valid_MPInt_Precision<PRECISION>
class MPInt;

// ^^^ MPInt was here only forward declared - because the Overflow_Error class
// needs to know about MPInt

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

  // ===== Getters =====
public:
  const _detail::MPInt_Value &value() const { return value_; }

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

  // convert from another precision - if new precision is lower will make MAX
  // value in current precision.
  // this is EXPLICIT on purpose to prevent unintentional precision loss
  template <std::size_t OTHER_PRECISION>
    requires _detail::Valid_MPInt_Precision<OTHER_PRECISION>
  explicit MPInt(const MPInt<OTHER_PRECISION> &number) {
    if constexpr (PRECISION == Unlimited) {
      value_ = number.value();
      return;
    }

    value_.digits_.resize(PRECISION, 0xFF);
    value_.is_positive_ = number.value().is_positive_;
  }

  // wrap the plain value with this MPInt class
  MPInt(const _detail::MPInt_Value &value) {
    check_overflow(value);
    value_ = std::move(value);
  }
  MPInt(_detail::MPInt_Value &&value) {
    check_overflow(value);
    value_ = std::move(value);
  }

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
      // copy the value to error
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
  operator+(const MPInt<OTHER_PRECISION> &other) const {
    MPInt<higher_precision<PRECISION, OTHER_PRECISION>> result(*this);
    result += other;
    return result;
  }

  // operator for subtracting: keep higher precision, is using -= internally
  template <std::size_t OTHER_PRECISION>
  MPInt<higher_precision<PRECISION, OTHER_PRECISION>>
  operator-(const MPInt<OTHER_PRECISION> &other) const {
    MPInt<higher_precision<PRECISION, OTHER_PRECISION>> result(*this);
    result -= other;
    return result;
  }

  // operator for multiplying: keep higher precision, is using *= internally
  template <std::size_t OTHER_PRECISION>
  MPInt<higher_precision<PRECISION, OTHER_PRECISION>>
  operator*(const MPInt<OTHER_PRECISION> &other) const {
    MPInt<higher_precision<PRECISION, OTHER_PRECISION>> result(*this);
    result *= other;
    return result;
  }

  // operator for dividing: keep higher precision, is using /= internally
  template <std::size_t OTHER_PRECISION>
  MPInt<higher_precision<PRECISION, OTHER_PRECISION>>
  operator/(const MPInt<OTHER_PRECISION> &other) const {
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

// convert MPInt number to base10 numeric string
template <std::size_t P>
std::ostream &operator<<(std::ostream &os, const MPInt<P> &number) {
  return os << number.to_string();
}

// was declared in Overflow_Error, but due to it not knowing the templated class
// MPInt, it must be defined here, after the MPInt class definition
inline MPInt<Unlimited> Overflow_Error::unlimited_value() const {
  // use constructor from MPInt to construct MPInt from MPInt_Value
  return MPInt<Unlimited>(value_);
}

} // namespace MPInt
