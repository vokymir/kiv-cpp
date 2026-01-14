
#include "Canvas.hpp"
#include "I_O.hpp"
#include "Util.hpp"
#include <exception>
#include <filesystem>
#include <iostream>

int main(int argc, char **argv) {
  std::filesystem::path source, target;
  int width, height, read_lines;

  try {
    Util::load_args(argc, argv, source, target, width, height);
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
