#include <iostream>
#include <thread>
#include <mutex>

// Suppose we have a class:
struct A {
  // That has a variable,
  int m_count;
  // that is accessed simulateously by two threads.
  // Then we need to protect that variable using a mutex, say this one:
  std::mutex m_count_mutex;
  // Because, when we don't use locking, then things go wrong.
};

// For convenience, lets instantiate an object of type A globally,
// even though that is normally evil.
A a;

// Consider this thread that accesses m_count without locking!
void thr_no_lock()
{
  // Loop 100000 times, each loop incrementing m_count by one.
  for (int i = 0; i < 1000000; ++i)
  {
    // Read m_count.
    int read = a.m_count;
    // Add 1 and write it back.
    a.m_count = read + 1;
  }
  // If the reading and writing was atomic then
  // this loop will have added precisely 100000.
}

// Also have a thread that does the same but with locking:
void thr_with_lock()
{
  for (int i = 0; i < 1000000; ++i)
  {
    std::unique_lock<std::mutex> lk(a.m_count_mutex);
    int read = a.m_count;
    a.m_count = read + 1;
  }
}

int main()
{
  // Do the test without locking first.
  a.m_count = 0;
  {
    std::thread t1(thr_no_lock);
    std::thread t2(thr_with_lock);

    // Wait for t1 and t2 to be finished.
    t1.join();
    t2.join();
  }

  // This result will be LESS THAN 200000!
  std::cout << "Afterwards the value of m_count, without locking, is: " << a.m_count << std::endl;

  // And again, but now both threads use locking.
  a.m_count = 0;
  {
    std::thread t1(thr_with_lock);
    std::thread t2(thr_with_lock);

    t1.join();
    t2.join();
  }

  // This result will be precisely 200000!
  std::cout << "Afterwards the value of m_count, with locking, is: " << a.m_count << std::endl;

  // Can you understand why?
}
