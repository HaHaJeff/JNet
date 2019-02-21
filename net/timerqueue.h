#ifndef TIMEQUEUE_H
#define TIMEQUEUE_H

#include <map>
#include <set>
#include <vector>

#include "util.h"
#include "timestamp.h"
#include "timer.h"
#include "channel.h"

class EventLoop;
class TimerId;

class TimerQueue : public Noncopyable
{
  public:
    explicit TimerQueue(EventLoop *loop);
    ~TimerQueue();

    TimerId AddTimer(const TimerCallback &cb, TimeStamp expir, double interval);
    TimerId AddTimer(TimerCallback &&cb, TimeStamp expir, double interval);

    void Cancel(TimerId timerId);

  //private:
    typedef std::pair<TimeStamp, Timer *> Entry;
    typedef std::set<Entry> TimerList;
    typedef std::pair<Timer *, int64_t> ActiveTimer;
    typedef std::set<ActiveTimer> ActiveTimerSet;

    void AddTimerInLoop(Timer *timer);
    void CancelTimerIdInLoop(TimerId timerId);

    void HandleRead();
    std::vector<Entry> GetExpired(TimeStamp now);
    void Reset(const std::vector<Entry> &expired, TimeStamp now);

    bool Insert(Timer *timer);

  private:
    EventLoop *loop_;
    const int timerfd_;
    Channel channel_;

    //key = expiration, value = timer*
    TimerList timers_;

    /*
    ActiveTimerSet avtiveTimers_;
    bool callingExpiredTimers_;
*/
    ActiveTimerSet cancelingTimers_;
};

#endif