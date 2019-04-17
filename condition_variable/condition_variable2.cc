// http://www.cplusplus.com/reference/condition_variable/condition_variable/
#include <iostream>
using std::cout; using std::endl;
#include <mutex>
using std::mutex; using std::lock_guard; using std::unique_lock;
#include <string>
using std::string;
#include <thread>
using std::thread; using std::this_thread::sleep_for;
#include <condition_variable>
using std::condition_variable;
#include <chrono>
#include <vector>
using std::vector;

mutex mtx;
condition_variable cv;
bool ready = false;

void print_id(int id) {
    unique_lock<mutex> lck(mtx);
    while (!ready) {
        cv.wait(lck);
    }
    cout << "thread " << id << endl;
}

void go() {
    unique_lock<mutex> lck(mtx);
    ready = true;
    cout << "go() notify all threads" << endl;
    cv.notify_all();
}

int main ()
{
  std::thread threads[10];
  // spawn 10 threads:
  for (int i=0; i<10; ++i)
    threads[i] = std::thread(print_id,i);

  std::cout << "10 threads ready to race...\n";
  go();                       // go!

  for (auto& th : threads) th.join();

  return 0;
}