// https://www.cnblogs.com/racaljk/p/8970847.html
#include <iostream>
using std::cout; using std::endl;
#include <future>
using std::future;
using std::async;

void task() {
    for (int i = 0; i < 3; ++i) {
        cout << "task()" << endl;
    }
}

int main() {
    
    // std::async创建一个后台线程执行传递的任务，
    // 这个任务只要是callable object均可，然后返回一个std::future
    future<void> result { async(task) };
    for (int i = 0; i < 2; ++i) {
        cout << "main()" << endl;
    }
    // future储存一个多线程共享的状态, 当调用future.get时
    // 强制阻塞main线程，直到task线程执行完毕
    result.get();
    // 打印结果：
    // main()
    // main()
    // task()
    // task()
    // task()

    return 0;
}