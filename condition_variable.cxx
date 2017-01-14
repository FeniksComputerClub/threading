#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>

// Lets use the same program as in use_lock.cxx.
// But this time we don't even have a mutex to protect m_count,
// because I want to demonstrate the use of a condition variable.
struct A {
  int m_count;
};

A a;

// A condition variable allows use to let a thread wait
// for a condition that is triggered by another thread.
// For example, one thread can be waiting for a boolean
// `ready' to be set to true, while another thread
// actually sets it to true.

// This variable that is used for the condition (this
// is actually the condition variable, but then it gets
// confusing) needs to be protected by a mutex again:

class B {
  private:
    bool m_ready;
    std::mutex m_ready_mutex;

    // The std::condition_variable is associated with
    // this mutex, so lets declare it inside B too:
    std::condition_variable m_ready_cv;

  public:
    B() : m_ready(false) { }

    void wait();
    void signal();

    static void thr_no_lock(B* b);
};

// Wait for 'm_ready' to become true.
void B::wait()
{
  std::unique_lock<std::mutex> lk(m_ready_mutex);
  m_ready_cv.wait(lk, [this]{return m_ready;});
  m_ready = false;
}

void B::signal()
{
  {
    // Officially we need to grab the mutex before accessing m_ready.
    std::unique_lock<std::mutex> lk(m_ready_mutex);
    m_ready = true;
  }
  // Wake up a (or the other) thread.
  m_ready_cv.notify_one();
}

// Now we're going to make this work without locking m_count
// by using the following algorithm: thread t1 and t2 run
// the same function and do this: one waits for the other
// to signal that they are ready (for the other to run) and
// then start running and increment m_count. After they
// incremented it, they signal that now, they are ready.

//static
void B::thr_no_lock(B* b)
{
  // Loop 100000 times, each loop incrementing m_count by one.
  for (int i = 0; i < 100000; ++i)
  {
    // Wait for the signal that the other thread is ready (waiting).
    b->wait();

    int read = a.m_count;
    a.m_count = read + 1;

    // Signal the other thread that we are done.
    b->signal();
  }
}

int main()
{
  B b;

  // Do the test without locking first.
  a.m_count = 0;
  {
    std::thread t1(B::thr_no_lock, &b);
    std::thread t2(B::thr_no_lock, &b);

    // Lets wait a bit to be sure that both
    // are running and waiting.
    std::this_thread::sleep_for(std::chrono::seconds(1));

    // At this point both threads are waiting,
    // so lets wake up one of them:
    b.signal();

    // Wait for t1 and t2 to be finished.
    t1.join();
    t2.join();
  }

  std::cout << "Afterwards the value of m_count, without locking, is: " << a.m_count << std::endl;

  // What is the result? Can you think of a reason why
  // the result could be less than 200000?
  //
  // What if we also run t3, t4 and t5 and then call b.signal() five times
  // instead of once?
}
