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

void start(std::thread& thread)
{
  std::cout << "Starting thread t1" << std::endl;
  std::thread t1(t1_prog);

  // The following allows us to destruct t1 without throwing an exception.
  // t1.detach();

  // Alternatively we can call t1.join() to wait for the thread to finish.
  // t1.join();

  // Or, we can swap t1 with another std::thread that CAN be destructed.
  thread.swap(t1);
}

int main()
{
  std::cout << "Entering main()" << std::endl;

  std::thread t1;       // Not associated with any thread.
  start(t1);            // t1 is swapped with another (newly created) thread.

  std::cout << "Sleeping for 10 seconds..." << std::endl;
  std::this_thread::sleep_for(std::chrono::seconds(10));

  // And again, before destructing t1, we need to call join() or detach() first.
  t1.detach();

  // Leaving main will destruct the running (detached) thread t1.
  std::cout << "Leaving main()" << std::endl;
}
