#include <algorithm>
#include <chrono>
#include <condition_variable>
#include <execution>
#include <future>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

/*
 * Preklad pomoci:
 * g++ -std=c++23 program.cpp -ltbb -o main
 *
 * ten flag -ltbb je dulezitej, jinak stena erroru
 * */

int main() {

  {

    std::vector<double> cisla;
    cisla.resize(100'000'000);

    auto tp_start = std::chrono::high_resolution_clock::now();

    std::fill(cisla.begin(), cisla.end(), 10.0);

    auto result = std::reduce(cisla.begin(), cisla.end());

    auto tp_end = std::chrono::high_resolution_clock::now();
    auto diff = tp_end - tp_start;
    auto ms =
        std::chrono::duration_cast<std::chrono::milliseconds>(diff).count();

    std::cout << "SECTENO, PODTRZENO: " << result << std::endl;
    std::cout << "TRVALO TO: " << ms << " ms" << std::endl;
  }

  {

    std::vector<double> cisla;
    cisla.resize(100'000'000);

    auto tp_start = std::chrono::high_resolution_clock::now();

    std::fill(std::execution::par_unseq, cisla.begin(), cisla.end(), 10.0);

    auto result =
        std::reduce(std::execution::par_unseq, cisla.begin(), cisla.end());

    auto tp_end = std::chrono::high_resolution_clock::now();
    auto diff = tp_end - tp_start;
    auto ms =
        std::chrono::duration_cast<std::chrono::milliseconds>(diff).count();

    std::cout << "SECTENO, PODTRZENO: " << result << std::endl;
    std::cout << "TRVALO TO: " << ms << " ms" << std::endl;
  }

  return 0;
}
