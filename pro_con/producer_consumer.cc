// https://segmentfault.com/a/1190000006703543
#include <iostream>
using std::cout; using std::endl;
#include <mutex>
using std::mutex; using std::lock_guard; using std::unique_lock;
#include <string>
using std::string;
#include <thread>
using std::thread; using std::this_thread::sleep_for;
#include <condition_variable>
using std::condition_variable;
#include <chrono>
#include <vector>
using std::vector;
#include <cstddef>
using std::size_t;
// 生产者 - 消费者（Producer-Consumer），也叫有限缓冲（Bounded-Buffer），是多线程同步的经典问题之一。
#define DISALLOW_COPY_AND_ASSIGN(ClassName) \
  ClassName(const ClassName&) = delete;     \
  void operator=(const ClassName&) = delete;

// class BoundedBuffer {
struct BoundedBuffer {
  public:
    BoundedBuffer(size_t size) 
      : begin_(0), end_(0), buffered_(0), circular_buffer_(size) {}
    void Produce(int n);
    int Consume();
    DISALLOW_COPY_AND_ASSIGN(BoundedBuffer);
  // private:
    size_t begin_;
    size_t end_;
    size_t buffered_;
    //           circular_buffer_;
    // begin_                                              end_
    // consume <- [int][int][int][int][int][int][int] <- produce
    vector<int> circular_buffer_;
    condition_variable not_full_cv_;
    condition_variable not_empty_cv_;
    mutex mutex_;
};
// cv.wait(lock, [] { return ready; }); 相当于：while (!ready) { cv.wait(lock); }。
void BoundedBuffer::Produce(int n) {
  std::cout << "BoundedBuffer::Produce" << std::endl;
  {
    unique_lock<mutex> lock(mutex_);
    // 缓冲区满，生产者等待
    while (buffered_ >= circular_buffer_.size()) {
      not_full_cv_.wait(lock);
    } // 或not_full_cv_.wait(lock, [=]{ return buffered_ < circular_buffer_.size(); });

    // 插入新的元素，更新下标
    circular_buffer_[end_] = n;
    for (auto c : circular_buffer_)
      cout << "circular_buffer_ elements: " << c << endl;
    // (end_ + 1) < circular_buffer_.size() 时，等号左边end_ = circular_buffer_.size()
    // (end_ + 1) == circular_buffer_.size() 时，等号左边end_ = 0
    // (end_ + 1) > circular_buffer_.size() 时，等号左边 = end_ + 1 - circular_buffer_.size()
    // 注意,vector的下标[]不能添加元素
    end_ = (end_ + 1) % circular_buffer_.size(); // % 取余数
    std::cout << "end_: " << end_ << endl;
    ++buffered_;
  } // 通知前，自动解锁。
  // 通知消费者。
  not_empty_cv_.notify_one();
}

int BoundedBuffer::Consume() {
  std::cout << "BoundedBuffer::Consume" << std::endl;
    unique_lock<mutex> lock(mutex_);
    // 缓冲区为空时，消费者等待
    while (buffered_ <= 0) {
      not_empty_cv_.wait(lock);
    } //或 not_empty_cv_.wait(lock, [=] { return buffered_ > 0; });

    // 移除一个元素。
    int n = circular_buffer_[begin_];
    begin_ = (begin_ + 1) % circular_buffer_.size();
    std::cout << "begin_: " << begin_ << endl;
    --buffered_;
    // 通知前，手动解锁。
    lock.unlock();
    // 通知生产者。
    not_full_cv_.notify_one();
    return n;
}

BoundedBuffer g_buffer(2);
mutex g_io_mutex;


// 生产 100000 个元素，每 10000 个打印一次。
void Producer() {
  std::cout << "\t\tProducer" << std::endl;
    int n = 0;
    while (n < 3) {
        g_buffer.Produce(n);
        if ((n % 1) == 0) {
            unique_lock<mutex> lock(g_io_mutex);
            cout << "Produce: " << n << endl;
        }
        ++n;
    }
    g_buffer.Produce(-1);
}

// 每消费到 10000 的倍数，打印一次。
void Consumer() {
  std::cout << "\t\tConsumer" << std::endl;
  std::thread::id thread_id = std::this_thread::get_id();

  int n = 0;
  do {
    n = g_buffer.Consume();
    if ((n % 1) == 0) {
      unique_lock<mutex> lock(g_io_mutex);
      cout << "Consume: " << n << " (" << thread_id << ")" << endl;
    }
  } while (n != -1);  // -1 表示缓冲已达末尾。

  // 往缓冲里再放一个 -1，这样其他消费者才能结束。
  g_buffer.Produce(-1);
}

int main() {
  vector<thread> threads;
  cout << "main(), circular_buffer_.size(): " << g_buffer.circular_buffer_.size() << endl;
  threads.push_back(thread(&Producer));
  threads.push_back(thread(&Consumer));
  // threads.push_back(thread(&Consumer));
  // threads.push_back(thread(&Consumer));

  for (auto& t : threads) {
    t.join();
  }

  return 0;
}