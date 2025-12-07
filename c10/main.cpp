import epic_files;

#include <filesystem>
#include <iostream>
#include <string>

int main() {
  std::filesystem::path dir = ".";
  std::string extension = ".txt";
  std::string regex = "*s.cpp";

  auto extension_files = Find_Files_With_Extension(dir, extension);
  std::cout << "Soubory s příponou " << extension << std::endl;
  for (auto &p : extension_files) {
    std::cout << "  " << p << std::endl;
  }

  auto match_files = Find_Files_Containing(dir, regex);
  std::cout << "Soubory, jejichž název odpovídá regexu " << regex << std::endl;
  for (auto &p : match_files) {
    std::cout << "  " << p << std::endl;
  }

  return 0;
}
