#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>

// Suppose we have our own 'mutex' class;
// that is, a class with a lock() and unlock()
// method.

class MyMutex {
  private:
    std::mutex m_mutex;
  public:
    void lock() { m_mutex.lock(); }
    void unlock() { m_mutex.unlock(); }
};

int count;
MyMutex count_mutex;

// And we try to use this with a condition variable,
// then we can use std::condition_variable_any, and
// simply pass the mutex to it (instead of a std::unique_lock).
std::condition_variable_any count_cv;

// A thread.
void thread_start()
{
  // BEFORE calling wait, the mutex must always be locked!
  count_mutex.lock();
  // Calling wait() will check the condition and when false
  // will unlock the mutex and wait till notify_one was called.
  // When woken up it will obtain the mutex again by calling
  // lock() on it and check the condition again, etc.
  // If the condition is true, wait() will exit without
  // first unlocking the mutex.
  count_cv.wait(count_mutex, []{return count >= 10000;});  // This would NOT compile if count_cv was a std::condition_variable.
  // Remember the value of count.
  int val = count;
  // Unlock the mutex because we're done with count.
  count_mutex.unlock();

  // This prints a number a bit larger than or equal to 10000.
  std::cout << "val = " << val << std::endl;
}

int main()
{
  std::thread t1(thread_start);

  for (int i = 0; i < 100000; ++i)
  {
    // These three lines:
    count_mutex.lock();
    count = i;
    count_mutex.unlock();
    // Are equivalent with this:
    {
      std::unique_lock<MyMutex> lk(count_mutex);
      count = i;
      // because unique_lock is just a wrapper that
      // calls lock() and unlock() upon creation and destruction.
    }

    // count was changed, so wake up t1 and have it check the condition again.
    count_cv.notify_one();
  }

  t1.join();
}
