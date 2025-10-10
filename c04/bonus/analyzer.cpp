#include "analyzer.h"
#include <algorithm>
#include <cmath>
#include <fstream>
#include <functional>
#include <iterator>
#include <numeric>
#include <ranges>
#include <stdexcept>

CAnalyzer::CAnalyzer(std::vector<double> &&numbers) noexcept
    : mNumbers(numbers) {
  // NOTE: v teto metode nic nemente!
}

CAnalyzer CAnalyzer::Create(const std::string &fileName) {
  // NOTE: v teto metode nic nemente!

  std::vector<double> numbers;

  // otevre soubor
  std::ifstream ifs(fileName, std::ios::in | std::ios::binary);

  if (!ifs.is_open()) {
    throw std::invalid_argument{"Nelze otevrit soubor " + fileName};
  }

  // zjisti velikost a pole cisel zvetsi, aby se tam cisla vesla
  ifs.seekg(0, std::ios::end);
  numbers.resize(ifs.tellg() / sizeof(double));
  ifs.seekg(0, std::ios::beg);

  // precte obsah souboru
  ifs.read(reinterpret_cast<char *>(numbers.data()),
           numbers.size() * sizeof(double));

  // overi, ze se precetlo vsechno
  if (ifs.gcount() != numbers.size() * sizeof(double)) {
    throw std::runtime_error{"Nelze precist cisla ze souboru " + fileName};
  }

  return CAnalyzer(std::move(numbers));
}

void CAnalyzer::Cache_Chunk_Averages() {
  // TODO

  /*
   * Implementacni poznamka:
   * pomoci algoritmu, ranges a views se pokuste vypocitat prumery vsech balicku
   * hodnot hodnoty se v souboru nachazeji po 10, tj. pokud je v souboru 50
   * cisel, jde o 5 balicku Vytvorte si atribut v teto tride a predpocitejte do
   * nej prumery z techto balicku cisel (na prikladu vyse tedy vypoctete 5
   * prumeru)
   */

  if (mNumbers.empty())
    return;

  mChunk_Averages.clear(); // don't accidentally duplicate

  auto n_chunks =
      (mNumbers.size() + Measurements_Per_Chunk - 1) / Measurements_Per_Chunk;

  mChunk_Averages.reserve(n_chunks);

  // get chunks of M_P_Ch, with each can be worked separately
  auto const chunks = mNumbers | std::views::chunk(Measurements_Per_Chunk);

  // how to get average of one chunk
  auto getAvg = [this](const auto &chunk) {
    double sum = std::ranges::fold_left(chunk, 0.0, std::plus<double>());
    double avg = sum / std::ranges::distance(chunk);
    mChunk_Averages.push_back(avg); // and save it
  };

  std::ranges::for_each(chunks, getAvg);
}

double CAnalyzer::Get_Min_Chunk_Average() const {
  /*
   * Implementacni poznamka:
   * tohle jde udelat na jednu radku, tj.: return ...;
   */

  return mChunk_Averages.empty() // this formatting is just IDE default
             ? throw CAnalyzerException{"You must call 'Cache_Chunk_Averages' "
                                        "before "
                                        "calling 'Get_Min_Chunk_Average'!"}
             : *std::min_element(mChunk_Averages.begin(),
                                 mChunk_Averages.end()); // TODO
}

double CAnalyzer::Get_Max_Chunk_Average() const {
  /*
   * Implementacni poznamka:
   * tohle jde udelat na jednu radku, tj.: return ...;
   */

  return mChunk_Averages.empty()
             ? throw CAnalyzerException{"You must call 'Cache_Chunk_Averages' "
                                        "before "
                                        "calling 'Get_Max_Chunk_Average'!"}
             : *std::max_element(mChunk_Averages.begin(),
                                 mChunk_Averages.end()); // TODO
}

std::array<double, CAnalyzer::Measurements_Per_Chunk>
CAnalyzer::Get_Piecewise_Averages() {
  /*
   * Implementacni poznamka:
   * pomoci algoritmu, ranges a views se pokuste vypocitat prumery pres sloupce
   * hodnot, tj. jestlize se nachazi v souboru hodnoty po 10 cislech, vypoctete
   * 10 prumeru napr. v prvnim prumeru budou zahrnuty indexy prvku 0, 10, 20,
   * 30, ... ve druhem indexy prvku 1, 11, 21, 31, ...
   */

  auto n_chunks =
      (mNumbers.size() + Measurements_Per_Chunk - 1) / Measurements_Per_Chunk;

  std::array<double, CAnalyzer::Measurements_Per_Chunk> res;

  // for each position in chunk, create a view with stride and simply calculate
  // AVG. Distance is here, because theoretically the last chunk can be of
  // different length than the others.
  for (int i = 0; i < Measurements_Per_Chunk; i++) {
    auto rang = mNumbers | std::views::stride(Measurements_Per_Chunk + i);
    res[i] = (std::ranges::fold_left(rang, 0.0, std::plus<double>())) /
             (std::ranges::distance(rang.begin(), rang.end()));
  }

  return res; // TODO
}
