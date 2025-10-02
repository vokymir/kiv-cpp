#include <string>
#include <iostream>

class IDescribed{
public:
    virtual std::string Get_Description() const = 0;
};

class Dinner final : public IDescribed {
public:
    virtual std::string Get_Description() const override {
        return "VECERE";
    }

};

class Snack final : public IDescribed {
public:
    virtual std::string Get_Description() const override {
        return "SVACINA";
    }

};


int main(int argc, char** argv){

    IDescribed* a = new Dinner();
    IDescribed* b = new Snack();

    std::cout << a->Get_Description() << std::endl;
    std::cout << b->Get_Description() << std::endl;

    return 0;
}
