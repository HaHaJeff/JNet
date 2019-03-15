#include "eventloop_thread.h"
#include "eventloop.h"
#include <assert.h>

EventLoopThread::~EventLoopThread() {
    if (started_) {
        if (loop_ != nullptr) {
            loop_->Quit();
        }
        thread_.join();
    }
}

EventLoop* EventLoopThread::StartLoop() {
    assert(!started_);
    started_ = true;

    assert(loop_ == nullptr);
    thread_ = std::thread([this](){RunInThread();});
    latch_.Wait();
    
    assert(loop_ != nullptr);
    return loop_;
}

void EventLoopThread::RunInThread() {
    EventLoop loop;
    loop_ = &loop;
    latch_.Count();
    loop.Loop();
    loop_ = nullptr;
}
