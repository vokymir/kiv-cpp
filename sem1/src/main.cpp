
#include "Canvas.hpp"
#include "I_O.hpp"
#include <exception>
#include <filesystem>
#include <iostream>
#include <stdexcept>

// Load all args into according variables, throw if argc < 4 or bad (incorrect)
// args.
void load_args(int argc, const char **argv, std::filesystem::path &source,
               std::filesystem::path &target, int &width, int &height);

// Entry point to app. Depending on args will draw SVG or PGM into desired file.
// If everything OK, will write OK and number of processed lines. If any error,
// write the error and return with 1.
int main(int argc, char **argv) {
  std::filesystem::path source, target;
  int width, height, read_lines;

  try {
    load_args(argc, const_cast<const char **>(argv), source, target, width,
              height);
    Canvas canvas(width, height);

    read_lines = I_O::parse_file(source, canvas);
    I_O::write(canvas, target);

  } catch (const std::exception &e) {

    std::cout << e.what() << std::endl;
    return 1;
  }

  std::cout << "OK\n" << read_lines << std::endl;
  return 0;
}

void load_args(int argc, const char **argv, std::filesystem::path &source,
               std::filesystem::path &target, int &width, int &height) {
  if (argc < 4) {
    throw std::runtime_error(
        "Usage:\n ./drawing.exe <input_file> <output_file> <width>x<height>");
  }

  source = argv[1];
  target = argv[2];

  std::string size{argv[3]};

  auto pos = size.find('x');
  if (pos == std::string::npos) {
    throw std::runtime_error("TODO: expected <w>x<h>");
  }

  width = std::stoi(size.substr(0, pos));
  height = std::stoi(size.substr(pos + 1));
}
