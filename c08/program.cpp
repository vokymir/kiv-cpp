#include <iostream>
#include <thread>

void psani_z_vlakna(char znak) {
  volatile int i = 0;
  while (true) {
    // std::cout << znak;
    i++;
  }
}

int main() {
  std::thread a(&psani_z_vlakna, 'X');
  std::thread b(&psani_z_vlakna, 'h');
  std::thread c(&psani_z_vlakna, 'r');
  std::thread d(&psani_z_vlakna, 'X');
  std::thread e(&psani_z_vlakna, 'h');
  std::thread f(&psani_z_vlakna, 'r');

  a.join();
  b.join();
  c.join();
  d.join();
  e.join();
  f.join();

  return 0;
}
