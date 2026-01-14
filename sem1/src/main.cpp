
#include "Canvas.hpp"
#include "Parser.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
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

  std::cout << canvas.draw_svg() << std::endl;

  return 0;
}
