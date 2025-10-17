#include "DataLoader.h"
#include <string>

bool Data_Loader::Load(const std::string &file) {
  // Hlavicka: x1,y1,x2,y2,color
  // cisla jsou integery, color je retezec (konkretne jeden z vyctu: black,
  // blue, green, yellow, red)

  std::ifstream in(file, std::ios::in);
  std::string x1_s, y1_s, x2_s, y2_s, color_s;
  char separator = ',';
  bool ret = false;

  std::getline(in, x1_s);

  // dokud se nacte cela cara
  while (std::getline(in, x1_s, separator) &&
         std::getline(in, y1_s, separator) &&
         std::getline(in, x2_s, separator) &&
         std::getline(in, y2_s, separator) && std::getline(in, color_s)) {
    int x1 = std::stoi(x1_s);
    int y1 = std::stoi(y1_s);
    int x2 = std::stoi(x2_s);
    int y2 = std::stoi(y2_s);
    std::string color = color_s;
    mLines.push_back({x1, y1, x2, y2, color});
    ret = true; // alespon jedna cara s bodikama se nacetla
  }

  return ret;
}

std::list<Data_Loader::Line>::const_iterator Data_Loader::begin() const {
  return mLines.begin();
}

std::list<Data_Loader::Line>::const_iterator Data_Loader::end() const {
  return mLines.end();
}
