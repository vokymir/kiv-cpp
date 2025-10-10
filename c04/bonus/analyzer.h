#pragma once

#include <array>
#include <stdexcept>
#include <string>
#include <vector>

// Pretty generic Exception, but it's obvious its from the CAnalyzer class.
class CAnalyzerException : public std::runtime_error {
public:
  CAnalyzerException(const std::string &msg) : std::runtime_error{msg} {}
};

class CAnalyzer final {

public:
  // velikost "balicku" mereni; pocet cisel obsazenych v souboru je vzdy
  // nasobkem tohoto cisla
  constexpr static size_t Measurements_Per_Chunk = 10;

protected:
  // konstruktor prebirajici nactena cisla
  CAnalyzer(std::vector<double> &&numbers) noexcept;

public:
  // staticka tovarni metoda - nacte cisla ze souboru, potencialne muze vyhodit
  // vyjimku; pokud je vse OK, vykonstruuje novou instanci CAnalyzer
  static CAnalyzer Create(const std::string &fileName);

  // predpocita a ulozi si do atributu prumerne hodnoty pres kazdy balicek
  // hodnot
  void Cache_Chunk_Averages();

  // vrati nejmensi z prumeru balickovych hodnot
  double Get_Min_Chunk_Average() const;
  // vrati nejvetsi z prumeru balickovych hodnot
  double Get_Max_Chunk_Average() const;

  // vypocte a vrati prumery pres sloupce
  std::array<double, Measurements_Per_Chunk> Get_Piecewise_Averages();

private:
  const std::vector<double> mNumbers;

  // TODO: sem patri urcite alespon jeden dalsi atribut
  std::vector<double> mChunk_Averages;
};
