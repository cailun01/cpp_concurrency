#include <iostream>
using std::cout; using std::cin; using std::endl;
#include <thread>
using std::thread; using std::this_thread::sleep_for;
#include <chrono>
using std::chrono::seconds;
#include <future>
using std::future; using std::promise;
#include <functional>     
using std::ref;

void task(promise<int>& p) {
    // future对象负责取回promise对象设置的值，task线程
    future<int> fu = p.get_future(); // 获取
    std::cout << "Retrieve value from another thread:" << fu.get() << "\n";
}

int main() {
    // promise对象是异步提供者(asynchronous provider) 
    // 为共享状态(shared state)设置一个值
    promise<int> p;
    thread td(task, ref(p));
    cout << "Main starting sleep for 5 seconds.\n";
    std::this_thread::sleep_for(std::chrono::seconds(5));
    cout << "Task in main thread set_value: 1024\n";
    p.set_value(1024); // 主线程设置一个值
    td.join();
    return 0;
}