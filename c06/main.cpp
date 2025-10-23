#include <iostream>
#include <memory>

class Integer {
private:
  int mVal;

public:
  Integer(int val) : mVal(val) {
    std::cout << "KONSTRUKCE " << mVal << std::endl;
  }
  Integer(Integer &&other) noexcept : mVal(other.mVal) {
    other.mVal = 0;
    std::cout << "MOVE KONSTRUKCE " << mVal << std::endl;
  }
  Integer &operator=(const Integer &other) = delete;

  Integer(const Integer &other) : mVal(other.mVal) {
    std::cout << "COPY KONSTRUKCE " << mVal << std::endl;
  }

  ~Integer() { std::cout << "DESTRUKCE " << mVal << std::endl; }

  int Get_Value() const { return mVal; }
};

template <typename T> void PrintLine(const T &p) {
  std::cout << p << std::endl;
}

int main() {
  Integer a(5);
  Integer b(a);
  Integer c(std::move(b));
  Integer d = c;

  std::unique_ptr<int> e;

  PrintLine("ahoj");
  PrintLine(342);
  PrintLine(2.3);

  std::cout << b.Get_Value() << std::endl;
}
