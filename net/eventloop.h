#ifndef EVENTLOOP_H
#define EVENTLOOP_H
#include "util.h"
#include "timestamp.h"
#include "timerid.h"
#include <functional>
#include <memory>
#include <vector>
#include <mutex>
#include <sys/types.h>

class Channel;
class Poller;
class TimerQueue;

// 从Poller中获取ActiveChannels
class EventLoop : public Noncopyable {
public:
    typedef std::function<void()> Functor;
    typedef std::function<void()> TimerCallback;

    EventLoop();
    ~EventLoop();

    void Loop();

    void UpdateChannel(Channel* ch);
    void RemoveChannel(Channel* ch);
    void AddChannel(Channel* ch);
    bool HasChannel(Channel* ch);

    void RunInLoop(const Functor& func);
    void RunInLoop(Functor&& func);
    void QueueInLoop(const Functor& func);
    void QueueInLoop(Functor&& func);
    void WakeUp();
    void AssertInLoopThread();
    bool IsInLoopThread();
    TimerId RunAt(TimeStamp&& time, TimerCallback&& cb);
    TimerId RunAt(const TimeStamp& time, const TimerCallback& cb);
    TimerId RunAfter(double delay, const TimerCallback& cb);
    TimerId RunAfter(double delay, TimerCallback&& cb);
    TimerId RunEvery(double delay, double interval, const TimerCallback& cb);
    TimerId RunEvery(double delay, double interval, TimerCallback&& cb);
    void Quit();
    pid_t GetTid() {return tid_;}

    void Cancel(TimerId timerId);
private:
    // for wake up;
    void HandleRead();
    void DoPendingFunctors();

private:
    typedef std::vector<Channel*> ChannelList;

    pid_t tid_;
    std::unique_ptr<Poller> poller_;
    std::unique_ptr<TimerQueue> timerQueue_;

    ChannelList activeChannels_;
    Channel* currentActiveChannel_;

    int wakeupFd_;
    std::unique_ptr<Channel> wakeupChannel_;

    bool quit_;
    static const int kPollTimeMs = 10000;
    std::vector<Functor> pendingFunctors_;
    bool doingPending_;
    std::mutex mutex_;
};

#endif
