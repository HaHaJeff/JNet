#ifndef EVENTLOOP_THREAD_H
#define EVENTLOOP_THREAD_H
#include "countdownlatch.h"
#include <thread>
class EventLoop;

class EventLoopThread : Noncopyable {
public:
    EventLoopThread() = default;
    ~EventLoopThread();
    EventLoop* StartLoop();
private:
    void RunInThread();

    bool started_ = false;
    EventLoop* loop_ = nullptr;
    std::thread thread_;
    CountDownLatch latch_{1};
};

#endif
