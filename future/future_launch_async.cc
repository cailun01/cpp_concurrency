// https://www.cnblogs.com/racaljk/p/8970847.html
#include <iostream>
using std::cout; using std::endl;
#include <future>
using std::future;
using std::async;
using std::launch;

void task() {
    for (int i = 0; i < 3; ++i) {
        cout << "task()" << endl;
    }
}

int main() {
    
    // 在future_get.cc中，上面task返回void，这个结果没用，我们只是单纯的想等待任务线程结束。
    // 对这种需求还可以用更简单的方法：在创建async的时候指定一个launch policy，不再需要get函数
    // 不过还是需要把async的返回值赋给result。
    // 如果不赋值async会和同步调用一样在这里阻塞直到调用完毕，相当于没用async。
    future<void> result { async(launch::async, task) };
    for (int i = 0; i < 2; ++i) {
        cout << "main()" << endl;
    }
    
    // 打印结果：注意到，打印结果没有严格的执行顺序了(future_get有)
    // main()
    // main()
    // task()
    // task()
    // task()
    // 或：
    // main()task()
    // task()
    // task()

    // main()
    // 或
    // task()
    // task()
    // task()
    // main()
    // main()

    return 0;
}