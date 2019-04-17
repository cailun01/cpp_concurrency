#include <iostream>
using std::cout; using std::endl;
#include <mutex>
using std::mutex; using std::lock_guard;
#include <thread>
using std::thread;
#include <vector>
using std::vector;
#include <cstddef>
using std::size_t;

mutex g_mutex;
int g_count = 0;
// 使用 lock_guard 自动加锁、解锁。原理是 RAII，和智能指针类似。
void Counter() {
    lock_guard<mutex> lck(g_mutex);
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