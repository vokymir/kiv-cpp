#include "drawing.h"

#include <algorithm>

#undef min
#undef max

void PPMDrawing::Init(int width, int height, const std::string &filename) {
  mFilename = filename;

  mBitmap.resize(height);
  for (auto &vec : mBitmap) {
    vec.resize(width);
    std::fill(vec.begin(), vec.end(), Color{255, 255, 255});
  }
}

void PPMDrawing::Save() {
  std::ofstream ofs(mFilename + ".ppm", std::ios::out | std::ios::binary);

  ofs << "P6" << std::endl;
  ofs << mBitmap[0].size() << " " << mBitmap.size() << std::endl;
  ofs << "255" << std::endl;

  // FIRST ATTEMP
  // for (auto &vecY : mBitmap) {
  //   for (auto &col : vecY) {
  //     ofs.write(col.data(), Color::size_bytes());
  //   }
  // }

  const size_t rowSize = mBitmap[0].size() * Color::size_bytes();
  for (auto &vecY : mBitmap) {
    // cause mBitmap is continuous, it works
    ofs.write(reinterpret_cast<const char *>(vecY.data()), rowSize);
  }
}

void PPMDrawing::Draw_Pixel(int x, int y, Color clr) {
  if (y < 0 || y >= static_cast<int>(mBitmap.size()))
    return;

  if (x < 0 || x >= static_cast<int>(mBitmap[y].size()))
    return;

  mBitmap[y][x] = clr;

  Save();
}
