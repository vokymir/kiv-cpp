

class Base {
public:
    Base();
    virtual ~Base();

    virtual void Say_Hello();
};

class Derived : public Base {
public:
    Derived();
    ~Derived();
    void Say_Hello() override;
};
