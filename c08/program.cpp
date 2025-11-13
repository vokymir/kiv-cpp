#include <condition_variable>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>

/*
 * Muze to nefungovat, pokud bude ctenar naplanovan jako prvni.
 * */

std::mutex mtx;
std::condition_variable cv;
int vysledek = -1;

void ctenar() {
  while (true) {
    std::unique_lock<std::mutex> lck(mtx);

    cv.wait(lck);

    std::cout << "MAM: " << vysledek << std::endl;

    cv.notify_one();
  }
}

void pisar() {

  for (int i = 0; i < 1000; i++) {
    std::unique_lock<std::mutex> lck(mtx);
    vysledek++;
    std::cout << "VYMYSLEL JSEM: " << vysledek << std::endl;

    cv.notify_one();

    cv.wait(lck);
  }
}

int main() {
  std::thread ct(&ctenar);
  std::thread pi(&pisar);

  pi.join();
  ct.join();

  return 0;
}
