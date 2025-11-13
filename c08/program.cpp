#include <iostream>
#include <mutex>
#include <string>
#include <thread>

std::mutex mtx;

void psani_z_vlakna(std::string slovo) {
  for (int i = 0; i < 300; i++) {

    mtx.lock();

    for (auto c : slovo) {
      std::cout << c;
    }
    std::cout << std::endl;

    mtx.unlock();
  }
}

int main() {
  std::thread a(&psani_z_vlakna, "ahoj");
  std::thread b(&psani_z_vlakna, "bojo");
  std::thread c(&psani_z_vlakna, "coho");

  psani_z_vlakna("TOHO");

  a.join();
  std::cout << "===== KONEC A =====" << std::endl;
  b.join();
  std::cout << "===== KONEC B =====" << std::endl;
  c.join();
  std::cout << "===== KONEC C =====" << std::endl;

  return 0;
}
