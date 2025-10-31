#include <array>
#include <ios>
#include <iostream>
#include <vector>
// vyzaduje >= C++23

// ===== bonus 1 =====
// nasel jsem si trochu dopredu koncepty, kdyz o nich porad rikate, jak jsou
// super

template <typename T>
concept IsIterable = requires(T t) {
  { std::begin(t) };
  { std::end(t) };
};

template <typename Item, IsIterable... Containers>
bool Any_Of_Contains(const Item &item, const Containers &...containers) {
  return ((std::find(std::begin(containers), std::end(containers), item) !=
           std::end(containers)) ||
          ...);
}

// ===== bonus 2 =====

template <typename... S> void Quoted_Print(const S &...s) {
  ((std::cout << "\"" << s << "\" "), ...) << std::endl;
}

// ===== main =====

int main() {
  std::vector<int> vec{3, 4, 6, 7, 43, 2, 4, 56, 7, 1};
  std::array<int, 3> arr{4, 5, 23};

  for (auto &i : {0, 1, 2}) {
    bool res = Any_Of_Contains(i, vec, arr);
    std::cout << "Any of contains for " << i << " returned: " << std::boolalpha
              << res << std::endl;
  }

  Quoted_Print("ahoj", 12, "baf", 4.2);
  return 0;
}
