#pragma once

#include "Canvas.hpp"
#include <cmath>
#include <filesystem>
#include <fstream>
#include <stdexcept>

class Util {
public:
  static bool is_zero(float f) {
    const float epsilon = 1e-6;
    return fabsf(f) < epsilon;
  }

  static void load_args(int argc, char **argv, std::filesystem::path &source,
                        std::filesystem::path &target, int &width,
                        int &height) {
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

  static void write(const Canvas &canvas, std::filesystem::path &target) {
    std::ofstream target_file{target};
    if (!target_file) {
      throw std::runtime_error("TODO: cannot write file.");
    }

    if (target.extension() == "svg") {
      target_file << canvas.draw_svg();

    } else if (target.extension() == "pgm") {
      target_file << canvas.draw_pgm();

    } else {
      throw std::runtime_error("TODO: invalid target file extension");
    }
  }
};
