#include <algorithm>
#include <cmath>
#include <exception>
#include <functional>
#include <iostream>
#include <map>
#include <ranges>
#include <stdexcept>
#include <string>
#include <vector>

// jeste random cisla jsou u Ubla na home, cviceni 4

// int main(int argc, char **argv) { return 0; }

class MojeVyjimka : public std::exception {
public:
  MojeVyjimka(const std::string &cosestalo) {
    // ommited
  }
  virtual const char *what() const noexcept override { return "Jejda!"; }
};

class MojeDruhaVyjimka : public std::runtime_error {
public:
  MojeDruhaVyjimka(const std::string &cosestalo)
      : std::runtime_error{cosestalo} {}
};

int main(int argc, char **argv) {

  try {

    std::vector<int> v;

    v.resize(1000000000000000000000000);

    throw MojeDruhaVyjimka{"No ale ale."};
    throw MojeVyjimka{"Haha"};
    throw std::runtime_error{"Neco se pokazilo."};
    throw "cokoliv";

  } catch (std::exception &ex) {
    std::cout << "VYJIMKA: " << ex.what() << std::endl;
  }

  catch (...) {
    std::cout << "Nevim, co prislo." << std::endl;
  }

  return 0;
}

// tohle se da pouzit na SEMESTRALKA 1
// int main(int argc, char **argv) {
//   std::map<std::string, std::function<std::string(std::string)>>
//   Handler_Table;
//
//   Handler_Table["Ahoj"] = [](std::string s) -> std::string { return "Nazdar";
//   }; Handler_Table["Marco"] = [](std::string s) -> std::string { return
//   "Polo"; };
//
//   while (1) {
//     std::string prikaz;
//
//     std::cin >> prikaz;
//
//     if (Handler_Table.contains(prikaz)) {
//
//       std::cout << Handler_Table[prikaz](prikaz) << std::endl;
//     }
//   }
//
//   return 0;
// }

// int main(int argc, char **argv) {
//   std::vector<int> cisla;
//
//   cisla.resize(10);
//
//   // musi byt predtim resize, jinak zapisuju kam nemam
//   std::generate(cisla.begin(), cisla.end(), [n = 10]() mutable { return n++;
//   });
//   // std::generate_n(cisla.begin(), 20, [n = 10]() mutable { return n++; });
//   //
//
//   std::transform(cisla.begin(), cisla.end(), cisla.begin(),
//                  [](int c) { return c * 10; });
//
//   for (auto c : cisla) {
//     std::cout << c << std::endl;
//   }
//
//   for (auto c : cisla | std::views::transform([](int c) { return c * 10; }) |
//                     std::views::drop(5) | std::views::reverse |
//                     std::views::take(2)) {
//     std::cout << c << std ::endl;
//   };
//
//   return 0;
// }

// int main(int argc, char **argv) {
//   auto pow2 = std::bind(std::powf, std::placeholders::_1, 2.0f);
//   auto dvana = std::bind(std::powf, 2.0f, std::placeholders::_1);
//   auto kilo = std::bind(std::powf, 2.0f, 4.0f);
//
//   std::cout << pow2(3.0f) << std::endl;
//   std::cout << dvana(3.0f) << std::endl;
//   std::cout << kilo() << std::endl;
//
//   return 0;
// }

// class Trida {
// public:
//   int atr = 90;
//
//   int fce() {
//     auto lambda = [this]() -> int { return atr + fce(); };
//
//     return lambda();
//   }
// };
//
// int p = 8;
//
// int main(int argc, char **argv) {
//
//   int a = 10;
//   const int b = 5;
//   int c = 60;
//
//   auto fnc = [&a, c, argc](int xx) -> double {
//     return a + b + c + p + argc + xx;
//   };
//
//   a = 999;
//
//   std::cout << fnc(9) << std::endl;
//
//   return 0;
// }
