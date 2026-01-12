
#include <filesystem>
#include <iostream>

int main(int argc, char **argv) {
  if (argc < 4) {
    std::cout << "Usage:\n ./drawing.exe <input_file> <output_file> <size>"
              << std::endl;
    return 1;
  }

  std::filesystem::path source(argv[1]);
  std::filesystem::path target(argv[1]);

  return 0;
}
