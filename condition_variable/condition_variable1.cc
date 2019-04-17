// https://segmentfault.com/a/1190000006679917
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

// 条件变量（Condition Variable）的一般用法是：线程 A 等待某个条件并挂起，
// 直到线程 B 设置了这个条件，并通知条件变量，然后线程 A 被唤醒。
// 经典的「生产者-消费者」问题就可以用条件变量来解决。

// 与条件变量搭配使用的「锁」，必须是 unique_lock，不能用 lock_guard。
// 等待前先加锁。等待时，如果条件不满足，wait 会原子性地解锁并把线程挂起。

// 条件变量被通知后，挂起的线程就被唤醒，但是唤醒也有可能是假唤醒，或者是因为超时等异常情况，
// 所以被唤醒的线程仍要检查条件是否满足，所以 wait 是放在条件循环里面。
// cv.wait(lock, [] { return ready; }); 相当于：while (!ready) { cv.wait(lock); }。

mutex mtx;
condition_variable cv;
string data;
bool ready = false;     // 条件
bool processed = false; // 条件

void worker_function() {
    unique_lock<mutex> lock(mtx);

    // 等待主线程发送数据。
    cv.wait(lock, [] { return ready; });
    cout << "worker thread is processing data." << endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
    data += "processed";
    processed = true;
    cout << "worker thread notify that data is processed." << endl;
    lock.unlock();
    cv.notify_one(); // 通知main()
}

int main() {
    thread worker(worker_function);
    {
        lock_guard<mutex> lock(mtx);
        cout << "main thread is preparing data." << endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
        data = "sample ";
        ready = true;
        cout << "Data prepared." << endl;
    }
    cv.notify_one(); // 通知worker_function()

    {
        unique_lock<mutex> lock(mtx);
        cv.wait(lock, [] {return processed;});
    }
    cout << "Return to main thread, data: " << data << endl;

    worker.join();
}

// main thread is preparing data.
// Data prepared.
// worker thread is processing data.
// worker thread notify that data is processed.
// Return to main thread, data: sample processed