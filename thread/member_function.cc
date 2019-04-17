// https://segmentfault.com/a/1190000016186741

#include <iostream>
using std::cout; using std::endl;
#include <thread>
using std::thread; 
#include <functional>
using std::bind;
#include <chrono>
using std::chrono::seconds;

// 通过一个成员函数创建线程。
// 与前例不同之处在于，需要以 bind 绑定 this 指针作为第一个参数。
class Hello {
  public:
    Hello() {
        // bind的第一个参数是指向函数的指针
        thread t(bind(&Hello::Entry, this, "World"));
        t.join();
    }
  private:
    // 线程函数
    void Entry(const char* what) {
        std::this_thread::sleep_for(std::chrono::seconds(3));
        cout << "Hello, " << what << "!" << endl;
    }
};

int main() {
    Hello hello;

    return 0;
}