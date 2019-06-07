#ifndef EVENTLOOP_POOL_H
#define EVENTLOOP_POOL_H
#include "util.h"
#include <memory>
#include <vector>

namespace jnet {

class EventLoop;
class EventLoopThread;

class EventLoopPool : Noncopyable {
public:
    EventLoopPool(EventLoop* baseLoop);
    ~EventLoopPool();
    void SetThreadNum(int num) {numThreads_ = num;} 
    void Start();

    // round-robin
    EventLoop* GetNextLoop();
    std::vector<EventLoop*> GetAllLoops();
    bool Started() const { return started_; }
private:
    EventLoop* baseLoop_;
    int numThreads_;
    bool started_;
    int next_;
    std::vector<std::shared_ptr<EventLoopThread>> threads_;
    std::vector<EventLoop*> loops_;
};
}
#endif
