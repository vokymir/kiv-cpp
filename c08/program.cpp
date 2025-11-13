#include <iostream>
#include <thread>

void psani_z_vlakna(char znak) {
  for (int i = 0; i < 300; i++) {
    std::cout << znak << std::endl;
  }
}

int main() {
  std::thread a(&psani_z_vlakna, 'a');
  std::thread b(&psani_z_vlakna, 'b');
  std::thread c(&psani_z_vlakna, 'c');

  psani_z_vlakna('_');

  a.join();
  std::cout << "===== KONEC A =====" << std::endl;
  b.join();
  std::cout << "===== KONEC B =====" << std::endl;
  c.join();
  std::cout << "===== KONEC C =====" << std::endl;

  return 0;
}
