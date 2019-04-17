// https://www.youtube.com/watch?v=eeSC43KQdVI&list=PL_dsdStdDXbrzGQUMh2sy6T8GcCCst3Nm
#include <iostream>
using std::cout; using std::endl;
#include <thread>
using std::thread;
#include <functional>     
using std::ref;

int main() {
    int local = 10;
    thread t1 {[](int& value) {
        cout << "Inside a thread t1" << endl;
        cout << "value: " << value << endl;
    }, ref(local)};

    thread t2 {[&](){
        cout << "Inside a thread t2" << endl;
        cout << "value: " << local++ << endl;
    }};
    t1.join();
    t2.join();
    cout << "main function local value: " << local << endl;

    return 0;
}