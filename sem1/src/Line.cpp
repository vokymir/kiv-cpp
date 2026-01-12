
#include "Line.hpp"
#include <string>

void Line::translate(int x, int y) {}
void Line::rotate(int x, int y, float a) {}
void Line::scale(int x, int y, float f) {}

std::string Line::draw_svg() { return "svg"; }
std::string Line::draw_pgm() { return "pgm"; }
