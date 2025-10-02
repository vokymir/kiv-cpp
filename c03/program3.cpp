#include <cmath>
#include <iostream>
#include <ostream>
#include <string>

class Zlomek{
private:
    int citatel;
    int jmenovatel;

public:
    Zlomek(int cit, int jm) : citatel(cit), jmenovatel(jm){}

    Zlomek operator*(const Zlomek& rhs){
        return Zlomek(citatel * rhs.citatel,
                      jmenovatel * rhs.jmenovatel);
    }

    std::string To_String() const {
        return std::to_string(citatel) + "/" + std::to_string(jmenovatel);
    }

    operator double() const {
        return (double)citatel / (double)jmenovatel;
    }

    operator bool() const {
        return citatel != 0;
    }

    // NEDELAT, NEDAVA SMYSL
    // void operator%(int a){
    //     std::cout << "Procento: " + std::to_string(double(this)) << std::endl;
    // }

    friend std::ostream& operator<<(std::ostream& os, const Zlomek& z);
};


std::ostream& operator<<(std::ostream& os, const Zlomek& z){
    os << z.To_String();
    return os;
    // a diky tomuto uz to nemusi byt a.To_String()
    // akoratze ja tam mam na levy strane string
}

int main(int argc, char** argv){
    Zlomek a(2,3);
    Zlomek b (1,5);

    Zlomek c = a * b;
    double vysledek = c;

    std::cout << "a = " + a.To_String() << std::endl;
    std::cout << "b = " + b.To_String() << std::endl;
    std::cout << "c = a * b = " + c.To_String() + " = " + std::to_string(vysledek) << std::endl;

    if (c){
        std::cout << "true" <<std::endl;
    }

    // c % 5;

    std::cout << c << std::endl; // tady to vklidu projde

    return 0;
}
