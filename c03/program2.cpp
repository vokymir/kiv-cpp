#include <iostream>

const int Prvni = time(nullptr);

constexpr int Nadruhou(int cilso){
    return cilso * cilso;
}

constexpr int Druha = Nadruhou(50);
constinit int Treti = Nadruhou(20); // C++ 20+

consteval int Natreti(int cislo){
    return cislo * cislo * cislo;
}

int main(int argc, char** argv){

    constexpr int Ctvrta = Nadruhou(30); // dobry
    const int Pata = Nadruhou(30); // vetsinou spatny
    int Sesta = Nadruhou(30); // vzdycky spatny

    std::cout << Nadruhou(argc) << std::endl;
    std::cout << Natreti(argc) << std::endl; // tohle uz by asi jit nemelo

    return 0;
}
