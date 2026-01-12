
#include "Canvas.hpp"
#include "Parser.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

int main(int argc, char **argv) {
  if (argc < 4) {
    std::cout
        << "Usage:\n ./drawing.exe <input_file> <output_file> <width>x<height>"
        << std::endl;
    return 1;
  }

  std::filesystem::path source{argv[1]};
  std::filesystem::path target{argv[2]};

  std::string size{argv[3]};

  auto pos = size.find('x');
  if (pos == std::string::npos) {
    std::cerr << "Invalid format, expected <width>x<height>\n";
    return 1;
  }

  int width = std::stoi(size.substr(0, pos));
  int height = std::stoi(size.substr(pos + 1));

  Canvas canvas(width, height);

  std::ifstream source_file(source);
  if (!source_file) {
    std::cerr << "Failed to open file\n";
    return 1;
  }

  Parser::parse_file(source_file, canvas);

  std::string line;
  while (std::getline(source_file, line)) {
    std::istringstream iss(line);

    std::string token;
    iss >> token;

    if (token == "line") {
      std::cout << "L" << std::endl;
    } else if (token == "circle") {
      std::cout << "C" << std::endl;
    } else if (token == "rect") {
      std::cout << "R" << std::endl;
    } else if (token == "translate") {
      std::cout << "TRANS" << std::endl;
    } else if (token == "rotate") {
      std::cout << "ROTAT" << std::endl;
    } else if (token == "scale") {
      std::cout << "SCALE" << std::endl;
    } else {
      std::cout << "WEIRD_LINE" << std::endl;
    }
  }

  return 0;
}
