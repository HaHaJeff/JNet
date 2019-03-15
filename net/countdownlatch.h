#ifndef COUNTDOWNLATCH
#define COUNTDOWNLATCH

#include <mutex>
#include <condition_variable>
#include "util.h"

class CountDownLatch : Noncopyable {
public:
    explicit CountDownLatch(int count) : count_(count) {}
    void Count() {
        std::lock_guard<std::mutex> guard(mutex_);
        count_--;
        if (count_ <= 0) { cond_.notify_all(); }
    }
    void Wait() {
        std::unique_lock<std::mutex> lock(mutex_);
        while(count_ > 0) cond_.wait(lock);
    }
private:
    int count_;
    std::mutex mutex_;
    std::condition_variable cond_;
};

#endif
