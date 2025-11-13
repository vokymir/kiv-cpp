#include <iostream>
#include <thread>

void psani_z_vlakna(char znak) {
  while (true) {
    std::cout << znak;
  }
}

int main() {
  std::thread a(&psani_z_vlakna, 'X');
  std::thread b(&psani_z_vlakna, 'h');
  std::thread c(&psani_z_vlakna, 'r');

  a.join();
  b.join();
  c.join();

  return 0;
}
