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

// 为输出流使用单独的 mutex。
// 这么做是因为 IO 流并不是线程安全的！
// 如果不对 IO 进行同步，此例的输出很可能变成：

// count == count == 2count == 41
// count == 3
// 因为在下面这条输出语句中：

// std::cout << "count == " << i << std::endl;
// 输出 "count == " 和 i 这两个动作不是原子性的（atomic），可能被其他线程打断。

// 2个互斥量
mutex g_mutex;
mutex g_io_mutex;
int g_count = 0;

void Counter() {
    int i;
    {
        std::unique_lock<std::mutex> lock(g_mutex);
        i = ++g_count;
    }

    {
        std::unique_lock<std::mutex> lock(g_io_mutex);
        std::cout << "count: " << i << std::endl;
    }
}

int main() {
    const std::size_t SIZE = 4;

    std::vector<std::thread> v;
    v.reserve(SIZE);

    for (std::size_t i = 0; i < SIZE; ++i) {
        v.emplace_back(&Counter);
    }

    for (std::thread& t : v) {
        t.join();
    }

    return 0;
}