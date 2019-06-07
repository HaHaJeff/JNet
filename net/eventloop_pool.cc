#include "eventloop_pool.h"
#include "eventloop.h"
#include "eventloop_thread.h"
#include <assert.h>

namespace jnet {

EventLoopPool::EventLoopPool(EventLoop* baseLoop)
    : baseLoop_(baseLoop),
      numThreads_(0),
      started_(false),
      next_(0)
{}

EventLoopPool::~EventLoopPool() {
}

void EventLoopPool::Start() {
    assert(!started_);
    baseLoop_->AssertInLoopThread();

    started_ = true;

    for(int i = 0; i < numThreads_; i++) {
        std::shared_ptr<EventLoopThread> s(new EventLoopThread());
        threads_.push_back(s);
        loops_.push_back(s->StartLoop());
    }
}

EventLoop* EventLoopPool::GetNextLoop() {
    baseLoop_->AssertInLoopThread();
    EventLoop* loop = baseLoop_;
    assert(started_);
    if (!loops_.empty()) {
        loop = loops_[next_];
        ++next_;
        if (next_ >= loops_.size()) next_ = 0;
    } 
    return loop;
}

std::vector<EventLoop*> EventLoopPool::GetAllLoops() {
    baseLoop_->AssertInLoopThread();
    assert(started_);
    if (loops_.empty()) {
        return std::vector<EventLoop*>(1, baseLoop_);
    }
    else {
        return loops_;
    }
}
}
