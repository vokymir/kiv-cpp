#include <iostream>
#include <string>

// int main(void){
//     int number;
//     do {
//         std::cout << "Choose a number between 1 and 5: ";
//         std::cin >> number;
//     } while (number < 1 || number> 5);  // Keep asking until the user enters a number between 1 and 5
//
//     std::cout << "You chose: " << number;
// }

// int main(void){
//     std::string name;
//     do {
//         std::cout << "Enter your name: ";
//         getline(std::cin, name);
//     } while (name.empty());  // Keep asking until the user enters something (name is not empty)
//
//     std::cout << "Hello, " << name;
// }

int main(void){
    int number;
    int *aha;
    std::cout << "Enter a number: ";
    while (!(std::cin >> number)) {  // Keep asking until the user enters a valid number
        std::cout << "Invalid input. Try again: ";
        std::cin.clear(); // Reset input errors
        std::cin.ignore(10000, '\n'); // Remove bad input
    }
    std::cout << "You entered: " << number;
}
