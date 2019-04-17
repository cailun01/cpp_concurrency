// https://segmentfault.com/a/1190000006614695
#include <iostream>
using std::cout; using std::endl;
#include <mutex>
using std::mutex;
#include <thread>
using std::thread;
#include <vector>
using std::vector;
#include <cstddef>
using std::size_t;

mutex g_mutex;
int g_count = 0;

// 直接操作 mutex，即直接调用 mutex 的 lock / unlock 函数。
void Counter() {
    g_mutex.lock();

    int i = ++g_count;
    cout << "count: " << i << endl;

    g_mutex.unlock();
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