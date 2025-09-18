#include "base.h"
#include <iostream>
#include <ostream>

Base::Base(){
    std::cout << "BASE-KONSTRUKTOR" << std::endl;
}

Base::~Base(){
    std::cout << "BASE-DESTRUKTOR" << std::endl;
}

void Base::Say_Hello(){
    std::cout << "BASE-HELLO" << std::endl;
}

Derived::Derived(){
    std::cout << "DERIVED-KONSTRUKTOR" << std::endl;
}

void Derived::Say_Hello(){
    std::cout << "DERIVED-HELLO" << std::endl;
}

Derived::~Derived(){
    std::cout << "DERIVED-DESTRUKTOR" << std::endl;
}
