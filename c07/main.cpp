#include <iostream>
#include <string>
#include <utility>

// CRTP

template <typename Child> class IPrinter {
public:
  Child &println(const std::string &str) {
    return static_cast<Child *>(this)->println(str);
  }
};

class ConsolePrinter : public IPrinter<ConsolePrinter> {
public:
  ConsolePrinter &println(const std::string &str) {
    std::cout << str << std::endl;
    return *this;
  }

  ConsolePrinter &SetSize(int w, int h) {
    std::cout << "SETTING NEW SIZE: " << w << "/" << h << std::endl;
    return *this;
  }
};

int main() {
  ConsolePrinter pr;

  pr.SetSize(10, 20)
      .println("ahoj")
      .println("svete")
      .println("dobryvecer")
      .SetSize(12, 13);
  return 0;
}

// IPRINTER jako DYNAMIC POLYMORPHISM
// class IPrinter {
// public:
//   virtual IPrinter &println(const std::string &str) = 0;
// };
//
// class ConsolePrinter : public IPrinter {
// public:
//   IPrinter &println(const std::string &str) override {
//     std::cout << str << std::endl;
//     return *this;
//   }
// };
//
// int main() {
//   ConsolePrinter pr;
//
//   pr.println("ahoj").println("svete").println("dobryvecer");
//   return 0;
// }

// FACTORY
// class Dog {
// public:
//   Dog() {}
//   Dog(const std::string &name) {}
//   Dog(const std::string &name, int race) {}
// };
//
// template <typename... Args> Dog DogFactory(Args &&...args) {
//   return Dog(std::forward<Args>(args)...);
// };
//
// int main(int argc, char **argv) {
//
//   auto dog1 = DogFactory("Alik");
//   auto pejsek = DogFactory("Egor", 2);
//
//   return 0;
// }

// VARIADIKY
// template <typename Arg0> int Sum(Arg0 arg0) { return arg0; }
//
// template <typename Arg0, typename... Args> double Sum(Arg0 arg0, Args...
// args) {
//   return arg0 + Sum(args...);
// }
//
// int main() {
//   auto result = Sum(1, 2, 3, 4, 5);
//
//   std::cout << result << std::endl;
//
//   return 0;
// }
