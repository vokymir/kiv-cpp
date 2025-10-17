#include "Drawing.h"
#include <format>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

/******************************************** SVG
 * *************************************************/

/*

Priklad SVG:

<svg viewBox="0 0 100 100" xmlns="http://www.w3.org/2000/svg">
    <line x1="0" y1="80" x2="100" y2="20" stroke="black" />
</svg>

*/

void SVGDrawing::Init(int width, int height) {
  //
  mPrototype << std::format(
      "<svg viewBox=\"0 0 {} {}\" xmlns=\"http://www.w3.org/2000/svg\">\n",
      width, height);
}

void SVGDrawing::Draw_Line(int fromX, int fromY, int toX, int toY, Color clr) {
  //
  std::string colorStr;
  switch (clr) {
  case Color::Black:
    colorStr = "black";
    break;
  case Color::Blue:
    colorStr = "blue";
    break;
  case Color::Green:
    colorStr = "green";
    break;
  case Color::Yellow:
    colorStr = "yellow";
    break;
  case Color::Red:
    colorStr = "red";
    break;
  }

  mPrototype << std::format(
      " <line x1=\"{}\" y1=\"{}\" x2=\"{}\" y2=\"{}\" stroke=\"{}\" />\n",
      fromX, fromY, toX, toY, colorStr);
}

void SVGDrawing::Save_To_File(const std::string &fileBaseName) {
  //
  mPrototype << "</svg>";

  std::ofstream out(fileBaseName + ".svg", std::ios::out);
  out << mPrototype.str();
}

/******************************************** PPM
 * *************************************************/

/*

Priklad PPM:

P6
100 100
255
<pole bajtu>



pole bajtu je posloupnost binarnich R, G a B slozek za sebou, pixely jdou zleva
doprava, shora dolu

*/

void PPMDrawing::Init(int width, int height) {
  //
}

void PPMDrawing::Draw_Line(int fromX, int fromY, int toX, int toY, Color clr) {
  //
}

void PPMDrawing::Save_To_File(const std::string &fileBaseName) {
  //
}
