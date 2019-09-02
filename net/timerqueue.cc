#include <sys/timerfd.h>
#include <unistd.h>

#include "timerqueue.h"
#include "log.h"
#include "timer.h"
#include "timerid.h"
#include "eventloop.h"

namespace jnet {

int CreateTimerfd()
{

    // create a new timer object, and returen a fd
    int timerfd = timerfd_create(CLOCK_MONOTONIC, TFD_CLOEXEC | TFD_NONBLOCK);
    FATALIF(timerfd < 0, "Failed in timerfd_create");
    return timerfd;
}

struct timespec HowMuchTimeFromNow(TimeStamp when)
{
    int64_t microseconds = when.MicroSecondsSinceEpoch() - TimeStamp::Now().MicroSecondsSinceEpoch();

    if (microseconds < 100)
    {
        microseconds = 100;
    }

    struct timespec ts;
    ts.tv_sec = static_cast<time_t>(
        microseconds / TimeStamp::kMicroSecondsPerSecond);
    ts.tv_nsec = static_cast<long>(
        (microseconds % TimeStamp::kMicroSecondsPerSecond) * 1000);

    return ts;
}

void ReadTimerfd(int timerfd, TimeStamp now)
{
    uint64_t howmany;
    ssize_t n = read(timerfd, &howmany, sizeof(howmany));

    TRACE("TimerQueue::HandleRead() %d at %s", howmany, now.ToFormattedString().c_str());

    if (n != sizeof(howmany))
    {
        ERROR("TimerQueue::HandleRead() reads %d bytes instead of 8", n);
    }
}

void ResetTimerfd(int timerfd, TimeStamp expiration)
{
    struct itimerspec newValue;
    struct itimerspec oldValue;
    bzero(&newValue, sizeof(newValue));
    bzero(&oldValue, sizeof(oldValue));
    newValue.it_value = HowMuchTimeFromNow(expiration);
    int ret = timerfd_settime(timerfd, 0, &newValue, &oldValue);
    if (ret)
    {
        ERROR("timerfd_settime()");
    }
}

TimerQueue::TimerQueue(EventLoop *loop)
    : loop_(loop),
      timerfd_(CreateTimerfd()),
      channel_(loop, timerfd_),
      timers_()
//      callingExpiredTimers_(false)
{
    TRACE("TimerQueue create, timerfd: %d", timerfd_);
    channel_.SetReadCallback(std::bind(&TimerQueue::HandleRead, this));
    //Add into poller
    //FIXME: 两次调用epoll_ctl系统调用，能否减少为一次
    //参见muduo的方式
    channel_.AddToLoop();
    channel_.EnableRead();
}

TimerQueue::~TimerQueue()
{
//    channel_.RemoveFromLoop();
    close(timerfd_);
    for (auto it : timers_)
    {
        delete it.second;
    }
}

TimerId TimerQueue::AddTimer(const TimerCallback &cb,
                             TimeStamp expiration,
                             double interval)
{
    Timer *timer = new Timer(cb, expiration, interval);
    loop_->RunInLoop(std::bind(&TimerQueue::AddTimerInLoop, this, timer));
    return TimerId(timer, timer->GetSequence());
}

TimerId TimerQueue::AddTimer(TimerCallback &&cb,
                             TimeStamp expiration,
                             double interval)
{
    Timer *timer = new Timer(cb, expiration, interval);
    loop_->RunInLoop(std::bind(&TimerQueue::AddTimerInLoop, this, timer));
    return TimerId(timer, timer->GetSequence());
}

void TimerQueue::Cancel(TimerId timerid)
{
    loop_->RunInLoop(std::bind(&TimerQueue::CancelTimerIdInLoop, this, timerid));
}

void TimerQueue::AddTimerInLoop(Timer *timer)
{
    loop_->AssertInLoopThread();
    // 如果当前timer的过期时间早于timers_中的最早的一个，则改变timerfd_的过期时间
    bool needReset = Insert(timer);

    if (needReset)
    {
        ResetTimerfd(timerfd_, timer->GetExpiration());
    }
}

void TimerQueue::CancelTimerIdInLoop(TimerId timerid)
{
    loop_->AssertInLoopThread();
    Timer *timer = timerid.timer_;
    if (timer == nullptr) return;
    TimeStamp expiration = timer->GetExpiration();
    timers_.erase(Entry(expiration, timer));
}

void TimerQueue::HandleRead()
{
    loop_->AssertInLoopThread();
    TimeStamp now(TimeStamp::Now());
    ReadTimerfd(timerfd_, now);

    //获得expired的Entry，并将其从timers_中erase
    std::vector<Entry> expired = GetExpired(now);
    TRACE("TimerQueue::HandleRead total count %d, now: %s", expired.size(), now.ToFormattedString().c_str());

    //因为HandleRead会将CancelInLoop调用，所以在Reset之前应该将cancelingTimers清空
    //Reset会将过期timer删除，如果该timer不在CancelInLoop中的话
    //如果在CancelTimerInLoop与GetExpired中重复删除，不会出现问题，因为在set以及map中
    //删除不存在的数据不会出现错误
    cancelingTimers_.clear();

    for (auto it : expired)
    {
        it.second->Run();
    }

    Reset(expired, now);
}

std::vector<TimerQueue::Entry> TimerQueue::GetExpired(TimeStamp now)
{
    std::vector<Entry> expired;
    Entry sentry(now, reinterpret_cast<Timer *>(UINTPTR_MAX));
    TimerList::iterator end = timers_.lower_bound(sentry);
    std::copy(timers_.begin(), end, back_inserter(expired));
    timers_.erase(timers_.begin(), end);
    return expired;
}

void TimerQueue::Reset(const std::vector<Entry> &expired, TimeStamp now)
{
    TimeStamp nextExpire;

    for (auto it : expired)
    {
        ActiveTimer timer(it.second, it.second->GetSequence());
        bool isCancelled = it.second->IsRepeated() && cancelingTimers_.find(timer) == cancelingTimers_.end();
        if (it.second->IsRepeated() && isCancelled)
        {
            //经过Interval之后重新启动定时器，并将其加入timers_中
            it.second->Restart(now);
            Insert(it.second);
        }
        else
        {
            delete it.second;
        }
    }

    if (!timers_.empty())
    {
        nextExpire = timers_.begin()->second->GetExpiration();
    }

    if (nextExpire.Valid())
    {
        ResetTimerfd(timerfd_, nextExpire);
    }
}

bool TimerQueue::Insert(Timer *timer)
{
    loop_->AssertInLoopThread();
    bool isEarliest = false;
    TimeStamp expiration = timer->GetExpiration();
    TimerList::iterator it = timers_.begin();
    if (it == timers_.end() || expiration < it->first)
    {
        isEarliest = true;
    }
    timers_.insert(Entry(expiration, timer));
    return isEarliest;
}
}
