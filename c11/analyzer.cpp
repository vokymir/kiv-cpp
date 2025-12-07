#include "analyzer.h"
#include <cstddef>
#include <vector>

#include "drawing.h"
#include "shared.h"
#include <fstream>

bool Dist2DAnalyzer::Load() {
  std::ifstream ifs(InFileName, std::ios::in | std::ios::binary);
  if (!ifs.is_open())
    return false;

  ifs.seekg(0, std::ios::end);
  mFile_Size = ifs.tellg();
  ifs.seekg(0, std::ios::beg);

  size_t count = mFile_Size / sizeof(double);
  mData = new double[count];
  size_t i = 0;
  while (ifs)
    ifs.read((char *)&mData[0], sizeof(double) * count);

  return true;
}

bool Dist2DAnalyzer::Analyze() {
  mMaximum = 0;

  // ensure at [x][y] exists
  if (Histogram.size() < ImgWidth) {
    Histogram.resize(ImgWidth);
  }
  for (auto &row : Histogram) {
    row.resize(ImgHeight, 0);
  }

  for (size_t i = 0; i + 1 < mFile_Size / sizeof(double); i += 2) {
    double d1 = mData[i];
    double d2 = mData[i + 1];

    size_t x = static_cast<size_t>((d1 - NumStart) / IntervalSizeX);
    size_t y = static_cast<size_t>((d2 - NumStart) / IntervalSizeY);

    if (x >= ImgWidth || y >= ImgHeight) {
      continue; // skip out-of-bounds
    }

    size_t value = ++Histogram[x][y];
    if (value > mMaximum) {
      mMaximum = value;
    }
  }

  return true;
}

bool Dist2DAnalyzer::Save_Image(const std::string &out) {
  PPMDrawing draw;
  draw.Init(ImgWidth, ImgHeight, out);
  for (size_t i = 0; i < Histogram.size(); i++) {
    for (size_t j = 0; j < Histogram[i].size(); j++) {
      double factor = 1.0 - (static_cast<double>(Histogram[i][j]) /
                             static_cast<double>(mMaximum));

      uint8_t grayscale = static_cast<uint8_t>(255.0 * factor);

      IDrawing::Color c{grayscale, grayscale, grayscale};

      draw.Draw_Pixel(static_cast<int>(i), static_cast<int>(j), c);
    }
  }
  draw.Save();

  return true;
}
