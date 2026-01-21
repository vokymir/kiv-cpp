#include "mpint.hpp"

int main(int argc, char **argv) {

  MPInt::MPInt<4> number("123456789012345678901234567890");
  MPInt::MPInt<MPInt::Unlimited> number2(1234567890);

  return 0;
}
