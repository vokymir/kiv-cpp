#include <cstdlib>
#include <iostream>
#include <array>
#include <iterator>
#include <string>
#include <utility>
#include <vector>
#include <map>
#include <set>
#include <memory>

class Rect {
public:
    int _w,_h;
    Rect(int w, int h) : _w{w}, _h{h} { std::cout << "KONSTRUKTOR w =" << _w <<", h = " << _h << std::endl; }
    ~Rect(){ std::cout << "DESTRUKTOR w =" << _w <<", h = " << _h << std::endl; }
};

void udelej_neco(std::unique_ptr<Rect>& h){}

int main(int argc, char* argv[]){

    std::unique_ptr<Rect> u = std::make_unique<Rect>(10,20);
    {
        std::unique_ptr<Rect> u2 = std::move(u);
        std::cout << "===" << std::endl;
    }

    {
        std::unique_ptr<Rect>& u3 = u;
        std::cout << "===" << std::endl;
        udelej_neco(u);
    }

    std::shared_ptr<Rect> s = std::make_shared<Rect>(7,14);
    {
        std::shared_ptr<Rect> s2 = std::move(u);
        std::cout << "===" << std::endl;
    }

    std::weak_ptr<Rect> w = s;

    std::cout << "KONEC" << std::endl;

    return EXIT_SUCCESS;
}

// TADY JEDEN START
// void describe(const std::vector<int>& vect){
//     std::cout << "SIZE: " << vect.size() << std::endl;
//     std::cout << "CAPACITY: " << vect.capacity() << std::endl;
// }
//
// int main(int argc, char* argv[]){
//     std::vector<int> cisla;
//
//     cisla.reserve(10); // mnohem lepsi
//
//     for (int i = 0; i < 10; i++){
//         cisla.push_back(i);
//         std::cout << "=====" << std::endl;
//         describe(cisla);
//     }
//
//     return EXIT_SUCCESS;
// }
// TADY JEDEN KONEC

// int main(int argc, char* argv[]){
//     // std::map<char*, std::string> cisla{ // nebezpecny
//     std::map<std::string, std::string> cisla{ // umi porovnavat stringy
//         {"a", "ahoj"},
//         {"b","zdar"},
//         {"c","cus"}
//     };
//
//     char c[2];
//
//     c[0] = 'a';
//     c[1] = '\0';
//
//     std::cout << cisla[c] << std::endl;
//
//     for (const auto& c : cisla){
//         std::cout << c.first << " = " << c.second << std::endl;
//     }
//
//     for (auto itr = cisla.begin(); itr != cisla.end(); ++itr){
//         auto& c = *itr;
//         std::cout << c.first << " = " << c.second << std::endl;
//     }
//
//     return EXIT_SUCCESS;
// }

// int main(int argc, char* argv[]){
//     std::map<int, std::string> cisla{
//         {3, "ahoj"},
//         {8,"zdar"},
//         {12,"cus"}
//     };
//
//     if (cisla.find(9) != cisla.end()){
//         std::cout << cisla[9] << std::endl; // kdyz neni guard, tak se vytvori prazdny zaznam
//     }
//
//     // for (const std::pair<const int, std::string>& c : cisla){ // LZE
//     // for (std::pair<const int, const std::string>& c : cisla){ // nelze
//     // for (std::pair< int, std::string>& c : cisla){ // nelze
//     for (std::pair<const int, std::string>& c : cisla){
//         std::cout << c.first << " = " << c.second << std::endl;
//     }
//
//     for (auto itr = cisla.begin(); itr != cisla.end(); ++itr){
//         auto& c = *itr;
//         std::cout << c.first << " = " << c.second << std::endl;
//     }
//
//     return EXIT_SUCCESS;
// }

// int main(int argc, char* argv[]){
//     // std::array<int, 3> cisla{3,8,12};
//     std::vector<int> cisla{3,8,12};
//
//     // for (const auto& c : cisla){
//     //     std::cout << c << std::endl;
//     // }
//
//     // ekvivalentni, dokonce to nahore se prepise na tohle (krome const, to nevim)
//     for (auto itr = cisla.begin(); itr != cisla.end(); ){
//         auto& c = *itr;
//         std::cout << c << std::endl;
//
//         if (c == 8){
//             itr = cisla.erase(itr); // ukazuje na nasledujici prvek
//         }
//         else {
//             ++itr;
//         }
//     }
//
//     return EXIT_SUCCESS;
// }

// int main(int argc, char* argv[]){
//     // std::array<int, 3> cisla{3,8,12};
//     std::vector<int> cisla{3,8,12};
//
//     for (const auto& c : cisla){
//         std::cout << c << std::endl;
//     }
//
//     // ekvivalentni, dokonce to nahore se prepise na tohle (krome const, to nevim)
//     for (auto itr = cisla.begin(); itr != cisla.end(); ++itr){
//         auto& c = *itr;
//         std::cout << c << std::endl;
//     }
//
//     return EXIT_SUCCESS;
// }


// int main(int argc, char* argv[]){
//
//     return EXIT_SUCCESS;
// }
