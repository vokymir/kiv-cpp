#include <condition_variable>
#include <future>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>

int main() {
  int local_result = 0;

  auto fut = std::async(std::launch::async, []() -> int {
    int async_result = 0;

    for (int i = 0; i < 1000000; i++) {
      async_result *= 2;
      async_result += 1;
    }

    return async_result;
  });

  for (int i = 0; i < 1000000; i++) {
    local_result *= 3;
    local_result -= 10;
  }

  int global_result = local_result + fut.get();

  std::cout << "VYSLEDEK: " << global_result << std::endl;

  return 0;
}
