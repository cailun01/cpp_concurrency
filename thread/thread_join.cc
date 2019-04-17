#include <iostream>
using std::cout; using std::endl;
#include <thread>
using std::thread;

void task() {
    for (int i = 0; i < 3; i++) {
        std::cout << "task()" << endl;
    }
}

int main() {
    std::thread td(task);
    for (int i = 0; i < 2; i++) {
        std::cout << "main()"<<endl;
    }
    td.join();
    return 0;
}