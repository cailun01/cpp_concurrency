// http://www.cnblogs.com/haippy/p/3252092.html
#include <cstddef>
using std::size_t;
#include <condition_variable>
using std::condition_variable;
#include <mutex>
using std::mutex; using std::lock_guard; using std::unique_lock;
#include <thread>
using std::thread; using std::this_thread::sleep_for;
#include <iostream>
using std::cout; using std::endl;

// 单生产者-单消费者模型中只有一个生产者和一个消费者，生产者不停地往产品库中放入产品，
// 消费者则从产品库中取走产品，产品库容积有限制，只能容纳一定数目的产品，如果生产者生产产品的速度过快，
// 则需要等待消费者取走产品之后，产品库不为空才能继续往产品库中放置新的产品，相反，如果消费者取走产品的速度过快，
// 则可能面临产品库中没有产品可使用的情况，此时需要等待生产者放入一个产品后，消费者才能继续工作。

// Item buffer size. 产品缓冲区
static const int kItemRepositorySize = 10;
// number of items we plan to produce.
static const int kItemsToProduce  = 1000;

struct ItemRepository {
    // 产品缓冲区, 配合 read_position 和 write_position 模型环形队列.
    int item_buffer[kItemRepositorySize];
    size_t read_position; // 消费者读取产品位置
    size_t write_position; // 生产则写入产品位置
    mutex mtx;             // 互斥量
    condition_variable repo_not_full;  // 条件变量，指示产品缓冲区不为满。
    condition_variable repo_not_empty; // 条件变量，指示产品缓冲区不为空。
} gItemRepository;

typedef struct ItemRepository ItemRepository;

void ProduceItem(ItemRepository* ir, int item) {
    unique_lock<mutex> lock(ir->mtx);
    // kItemRepositorySize = 10;
    while(((ir->write_position + 1) % kItemRepositorySize) ==
            ir->read_position) { // buffer已满，等待
        std::cout << "Producer is waiting for an empty slot...\n";
        (ir->repo_not_full).wait(lock);
    }
    (ir->item_buffer)[ir->write_position] = item; // 写入产品
    ++(ir->write_position);

    // 写入位置若是在队列最后则重新设置为初始位置.
    if (ir->write_position == kItemRepositorySize) 
        ir->write_position = 0;

    (ir->repo_not_empty).notify_all(); // 通知消费者产品库不为空.
    lock.unlock(); // 解锁.
}

int ConsumeItem(ItemRepository *ir) {
    int data;
    std::unique_lock<std::mutex> lock(ir->mtx);
    // item buffer is empty, just wait here.
    while(ir->write_position == ir->read_position) {
        std::cout << "Consumer is waiting for items...\n";
        (ir->repo_not_empty).wait(lock); // 消费者等待"产品库缓冲区不为空"这一条件发生.
    }

    data = (ir->item_buffer)[ir->read_position]; // 读取某一产品
    (ir->read_position)++; // 读取位置后移

    if (ir->read_position >= kItemRepositorySize) // 读取位置若移到最后，则重新置位.
        ir->read_position = 0;

    (ir->repo_not_full).notify_all(); // 通知消费者产品库不为满.
    lock.unlock(); // 解锁.

    return data; // 返回产品.
}

// 生产者任务
void ProducerTask() {
    for (int i = 1; i <= kItemsToProduce; ++i) {
        // sleep(1);
        std::cout << "Produce the " << i << "^th item..." << std::endl;
        ProduceItem(&gItemRepository, i); // 循环生产 kItemsToProduce 个产品.
    }
}

// 消费者任务
void ConsumerTask()  {
    static int cnt = 0;
    while(1) {
        int item = ConsumeItem(&gItemRepository); // 消费一个产品.
        std::cout << "Consume the " << item << "^th item" << std::endl;
        if (++cnt == kItemsToProduce) // kItemsToProduce  = 1000;
            break; // 如果产品消费个数为 kItemsToProduce, 则退出.
    }
}

void InitItemRepository(ItemRepository *ir) {
    ir->write_position = 0; // 初始化产品写入位置.
    ir->read_position = 0; // 初始化产品读取位置.
}

int main() {
    InitItemRepository(&gItemRepository);
    std::thread producer(ProducerTask); // 创建生产者线程.
    std::thread consumer(ConsumerTask); // 创建消费者线程.
    producer.join();
    consumer.join();
}