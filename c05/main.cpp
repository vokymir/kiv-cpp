
class A {
public:
  virtual void fncA() {}
  int a;
};

class B {
public:
  virtual void fncB() {}
  int b;
};

class C : public A {
public:
  void fncA() override {}
  int c;
};

int main() {
  C c;
  return 0;
}
