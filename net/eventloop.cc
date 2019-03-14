#include "eventloop.h"
#include "poller.h"
#include "timerqueue.h"
#include "channel.h"
#include <sys/eventfd.h>
#include <unistd.h>
#include <assert.h>
#include <iostream>

int CreateEventFd() {
    int evtfd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    FATALIF(evtfd < 0, "Failed in eventfd");
    return evtfd;
}

pid_t gettid() {
    pid_t p = static_cast<pid_t>(::syscall(SYS_gettid));
    return p;
}

EventLoop::EventLoop()
    : poller_(Poller::NewDefaultPoller(this)),
      tid_(gettid()),
      timerQueue_(new TimerQueue(this)),
      wakeupFd_(CreateEventFd()),
      wakeupChannel_(new Channel(this, wakeupFd_)),
      quit_(false),
      doingPending_(false)
{
    DEBUG("EventLoop created %p", this);
    wakeupChannel_->SetReadCallback(std::bind(&EventLoop::HandleRead, this));
    //FIXME: there has two epoll_ctl system call
    // can reduce to one time
    wakeupChannel_->AddToLoop();
    wakeupChannel_->EnableRead();
}

EventLoop::~EventLoop() {
    DEBUG("EventLoop %p destruct", this);
    wakeupChannel_->RemoveFromLoop();
    close(wakeupFd_);
}

void EventLoop::Loop() {
    TRACE("Event loop %p start looping", this);

    while(!quit_) {
        activeChannels_.clear();
        poller_->Poll(kPollTimeMs, &activeChannels_);

        for (auto ch : activeChannels_) {
            currentActiveChannel_ = ch;
            ch->HandleEvent();
        }
        currentActiveChannel_ = nullptr;
    }
}

// 对channel的操作一定实在io loop中的
// 其他线程通过RunInLoop调用
void EventLoop::UpdateChannel(Channel* ch) {
    AssertInLoopThread();
    poller_->UpdateChannel(ch);
}

void EventLoop::RemoveChannel(Channel* ch) {
    AssertInLoopThread();
    poller_->RemoveChannel(ch);
}

void EventLoop::AddChannel(Channel* ch) {
    AssertInLoopThread();
    poller_->AddChannel(ch);
}

bool EventLoop::HasChannel(Channel* ch) {
    return poller_->HasChannel(ch);
}

TimerId EventLoop::RunAt(TimeStamp&& time, TimerCallback&& cb) {
    return timerQueue_->AddTimer(cb, time, 0.0);
}

TimerId EventLoop::RunAt(const TimeStamp& time, const TimerCallback& cb) {
    return timerQueue_->AddTimer(cb, time, 0.0);
}

TimerId EventLoop::RunAfter(double interval, TimerCallback&& cb) {
    TimeStamp time(AddTime(TimeStamp(TimeStamp::Now()), interval));
    return timerQueue_->AddTimer(cb, time, 0.0);
}

TimerId EventLoop::RunAfter(double interval, const TimerCallback& cb) {
    TimeStamp time(AddTime(TimeStamp(TimeStamp::Now()), interval));
    return timerQueue_->AddTimer(cb, time, 0.0);
}

// TODO: thread safe?
void EventLoop::Quit() {
    assert(!quit_);
    quit_ = true;
    if (!IsInLoopThread()) {
        WakeUp();
    }
}

// FIXME:
void EventLoop::AssertInLoopThread() {
    assert(IsInLoopThread());
}

bool EventLoop::IsInLoopThread() {
    static thread_local pid_t t = gettid();
    return tid_ == t;
}

void EventLoop::RunInLoop(const Functor& func) {
    if (IsInLoopThread())
        func();
    else
        QueueInLoop(func);
}

void EventLoop::RunInLoop(Functor&& func) {
    func();
}

void EventLoop::QueueInLoop(const Functor& func) {
    {
        std::lock_guard<std::mutex> guard(mutex_);
        pendingFunctors_.push_back(func);
    }
    if (!IsInLoopThread() || doingPending_)
        WakeUp();
}

void EventLoop::QueueInLoop(Functor&& func) {
    {
        std::lock_guard<std::mutex> guard(mutex_);
        pendingFunctors_.push_back(std::move(func));
    }
    if (!IsInLoopThread() || doingPending_)
        WakeUp();
}

void EventLoop::WakeUp() {
    uint64_t one = 1;
    ssize_t n = ::write(wakeupFd_, &one, sizeof(one));
    if (n != sizeof(one)) {
        ERROR("EventLoop::WakeUp should write %lu bytes", sizeof(one));
    }
}

void EventLoop::HandleRead() {
    uint64_t one = 1;
    ssize_t n = read(wakeupFd_, &one, sizeof(one));
    if (n != sizeof(one)) {
        ERROR("EventLoop::HandleRead() reads %d bytes instead of 8", n);
    }
}

void EventLoop::Cancel(TimerId timerid) {
    timerQueue_->Cancel(timerid);
}

void EventLoop::DoPendingFunctors() {
    AssertInLoopThread();
    std::vector<Functor> funcs;
    {
        std::lock_guard<std::mutex> guard(mutex_);
        funcs.swap(pendingFunctors_);
    }
    doingPending_ = true;
    for (auto& func : funcs) {
        func();
    }
    doingPending_ = false;
}
