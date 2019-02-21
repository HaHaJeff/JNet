#ifndef EVENTLOOP_H
#define EVENTLOOP_H
#include "util.h"
#include "timestamp.h"
#include "timerid.h"
#include <functional>
#include <memory>
#include <vector>

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
    bool AssertInLoopThread();

    TimerId RunAt(TimeStamp&& time, TimerCallback&& cb);
    TimerId RunAt(const TimeStamp& time, const TimerCallback& cb);
    TimerId RunAfter(double delay, const TimerCallback& cb);
    TimerId RunAfter(double delay, TimerCallback&& cb);

    void Cancel(TimerId timerId);
private:
    // for wake up;
    void HandleRead();

private:
    typedef std::vector<Channel*> ChannelList;

    std::unique_ptr<Poller> poller_;
    std::unique_ptr<TimerQueue> timerQueue_;

    ChannelList activeChannels_;
    Channel* currentActiveChannel_;

    int wakeupFd_;
    std::unique_ptr<Channel> wakeupChannel_;

    bool quit_;
    static const int kPollTimeMs = 10000;
};

#endif