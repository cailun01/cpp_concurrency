//http://www.cnblogs.com/haippy/p/3252092.html
// 与单生产者和单消费者模型不同的是，单生产者-多消费者模型中可以允许多个消费者同时从产品库中取走产品。
// 所以除了保护产品库在多个读写线程下互斥之外，还需要维护消费者取走产品的计数器
#include <cstddef>
using std::size_t;
#include <unistd.h>
#include <cstdlib>
#include <condition_variable>
using std::condition_variable;
#include <iostream>
using std::cout; using std::endl;
#include <mutex>
using std::mutex; using std::lock_guard; using std::unique_lock;
#include <thread>
using std::thread; using std::this_thread::sleep_for;

static const int kItemRepositorySize = 4; // Item buffer size.
static const int kItemsToProduce = 10;    // How many items we plan to produce.

struct ItemRepository {
    int item_buffer[kItemRepositorySize];
    size_t read_position;
    size_t write_position;
    size_t item_counter;
    mutex mtx;
    mutex item_counter_mtx;
    condition_variable repo_not_full;
    condition_variable repo_not_empty;
} gItemRepository;

using ItemRepository = struct ItemRepository;

void ProduceItem(ItemRepository* ir, int item) {
    unique_lock<mutex> lock(ir->mtx);
    while (
        (ir->write_position + 1) % kItemRepositorySize 
        == ir->read_position
    ) {
        cout << "Producer is waiting for an empty slot ...\n";
        (ir->repo_not_full).wait(lock);
    }

    (ir)
}