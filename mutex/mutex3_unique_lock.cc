#include <iostream>
using std::cout; using std::endl;
#include <mutex>
using std::mutex; using std::unique_lock;
#include <thread>
using std::thread;
#include <vector>
using std::vector;
#include <cstddef>
using std::size_t;


mutex g_mutex;
int g_count = 0;
// 使用 unique_lock 自动加锁、解锁。
// unique_lock 与 lock_guard 原理相同，但是提供了更多功能（比如可以结合条件变量使用）。
// 注意：mutex::scoped_lock 其实就是 unique_lock<mutex> 的 typedef。
// 至于 unique_lock 和 lock_guard 详细比较，可移步 StackOverflow
// (http://stackoverflow.com/questions/6731027/boostunique-lock-vs-boostlock-guard)
void Counter() {
    unique_lock<mutex> lck(g_mutex);
    int i = ++g_count;
    cout << "count: " << i << endl;
}

int main() {
    size_t SIZE = 4;
    vector<thread> v;
    v.reserve(SIZE);

    for (size_t i = 0; i < SIZE; ++i) {
        v.emplace_back(&Counter);
    }
    for (std::thread& t : v) {
        t.join();
    }

    return 0;
}