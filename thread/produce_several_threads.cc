#include <thread>
#include <iostream>
#include <vector>
#include <algorithm>  // std::for_each
#include <functional> // std::mem_fn 
using namespace std;

void hello(int i) {
    cout << "hello: " << i << endl;
}

int main() {
    vector<thread> threads;
    for (unsigned i = 0; i < 5; ++i) {
        threads.push_back(thread(hello, i));
    }

    for_each(threads.begin(), threads.end(), 
        mem_fn(&thread::join));

    return 0;
}

// for_each的第三个形参是可调用对象，
// 迭代器解引用后，传入该可调用对象
// template<typename InputIterator, typename Function>
// Function for_each(InputIterator beg, InputIterator end, Function f) {
//   while(beg != end) 
//     f(*beg++);
// }

// mem_fn()把可以从成员指针生成一个可调用对象，
// 与function不同，mem_fn可以根据成员指针的类型推断可调用对象的类型，无序用户显式地指定
// 从成员函数指针生成可调用对象：function<>、mem_fn()和bind()
// https://www.cnblogs.com/bencai/p/9138463.html
// join是thread的成员函数，用mem_fn比较合适