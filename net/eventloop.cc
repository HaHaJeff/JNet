#include "eventloop.h"
#include "poller.h"
#include "timerqueue.h"
#include "channel.h"
#include <sys/eventfd.h>
#include <unistd.h>

int CreateEventFd() {
    int evtfd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);

    FATALIF(evtfd < 0, "Failed in eventfd");

    return evtfd;
}

EventLoop::EventLoop()
    : poller_(Poller::NewDefaultPoller(this)),
      timerQueue_(new TimerQueue(this)),
      wakeupFd_(CreateEventFd()),
      wakeupChannel_(new Channel(this, wakeupFd_)),
      quit_(false)
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

void EventLoop::UpdateChannel(Channel* ch) {
    poller_->UpdateChannel(ch);
}

void EventLoop::RemoveChannel(Channel* ch) {
    poller_->RemoveChannel(ch);
}

void EventLoop::AddChannel(Channel* ch) {
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

bool EventLoop::AssertInLoopThread() {
    return true;
}

void EventLoop::RunInLoop(const Functor& func) {
    func();
}

void EventLoop::RunInLoop(Functor&& func) {
    func();
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
