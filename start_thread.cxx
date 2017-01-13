#include <iostream>
#include <thread>
#include <chrono>

void t1_prog()
{
  std::chrono::seconds const one_second(1);

  // Run for a maximum of 100 seconds.
  for (int i = 0; i < 100; ++i)
  {
    std::cout << "\e[31mt1: " << i << "\e[0m" << std::endl;
    std::this_thread::sleep_for(one_second);
  }
}

void start_t1()
{
  std::cout << "Starting thread t1" << std::endl;
  std::thread t1(t1_prog);
  t1.detach();  // This allows us to destruct t1 without throwing an exception.
  // Alternatively we can call t1.join() to wait for the thread to finish.
}

int main()
{
  std::cout << "Entering main()" << std::endl;

  start_t1();

  std::cout << "Sleeping for 10 seconds..." << std::endl;
  std::this_thread::sleep_for(std::chrono::seconds(10));

  // Leaving main will destruct the running (detached) thread t1.
  std::cout << "Leaving main()" << std::endl;
}
