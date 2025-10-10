#include <fstream>
#include <iostream>

#include "analyzer.h"

/*
 * Domaci ukol:
 *
 * Implementujte chybejici tela metod Cache_Chunk_Averages,
 * Get_Min_Chunk_Average, Get_Max_Chunk_Average a Get_Piecewise_Averages tridy
 * CAnalyzer
 *
 * Jiz je pro vas pripraveno nacitani souboru, ktery obsahuje data z mereni (ve
 * skutecnosti systematicky nahodne vygenerovana data, ale to je vedlejsi)
 * Soubor obsahuje mereni ulozena v "balicku" po 10 (konstanta
 * CAnalyzer::Measurements_Per_Chunk), nacitani s tim ale nepocita - to proste
 * nacte vsechna data do jednoho velkeho pole, protoze je to tak nejvyhodnejsi
 * vzhledem k velikosti dat a vyuziti systemovych prostredku
 *
 * Vychozi stav je tedy pole mNumbers (ve tride CAnalyzer), ktere obsahuje
 * vsechna cisla, kterych je nasobek Measurements_Per_Chunk
 *
 * Vas ukol:
 *   1) Cache_Chunk_Averages - implementujte telo metody - tahle predpocita
 * vsechny prumerne hodnoty v ramci balicku hodnot 2) Get_Min_Chunk_Average,
 * Get_Max_Chunk_Average - implementujte tela metod - tyhle vrati nejnizsi,
 * resp. nejvyssi z prumeru predpocitanych vyse 3) Get_Piecewise_Averages -
 * implementujte telo metody - tahle spocte prumery ze vsech 1. mereni, 2.
 * mereni, ... tj. prumer ze vsech prvnich cisel z balicku, druhych, ... 4)
 * implementujte vlastni vyjimku, kterou budete vyhazovat, pokud nekdo zavola
 * Get_Min_Chunk_Average nebo Get_Max_Chunk_Average drive, nez
 * Cache_Chunk_Averages
 *      - tyto dve metody predpokladaji, ze jste uz prumery nechali spocitat
 *
 * Pro overeni: ocekavane hodnoty minima jsou cca 1.3 a maxima nad 10.0;
 * sloupcove prumery se pak budou pohybovat kolem cisla 6.0
 */

int main(int argc, char **argv) {

  // NOTE: v tomto souboru nic nemente!

  try {
    std::cout << "Nacitam soubor..." << std::endl;

    CAnalyzer analyzer = CAnalyzer::Create("04_data.bin");

    std::cout << "Analyzuji prumery..." << std::endl;

    analyzer.Cache_Chunk_Averages();

    std::cout << "Minimum prumeru: " << analyzer.Get_Min_Chunk_Average()
              << std::endl;
    std::cout << "Maximum prumeru: " << analyzer.Get_Max_Chunk_Average()
              << std::endl;

    auto piecewiseAvg = analyzer.Get_Piecewise_Averages();

    std::cout << "Prumerne sloupcove hodnoty: " << std::endl;
    for (size_t i = 0; i < CAnalyzer::Measurements_Per_Chunk; i++) {
      std::cout << "[" << i << "]: " << piecewiseAvg[i] << std::endl;
    }
  } catch (const std::exception &err) {
    std::cerr << "Vyskytla se chyba: " << err.what() << std::endl;
    return 1;
  }

  return 0;
}
