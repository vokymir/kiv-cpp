#include <iostream>
#include <ostream>
#include "base.cpp"

int main(int argc, char** argv){

    std::cout << "AAAA" << std::endl;

    {
        Base* trida = new Derived;
        trida->Say_Hello();
        delete trida;
    }

    std::cout << "ZZZ" << std::endl;

    return 0;
}
