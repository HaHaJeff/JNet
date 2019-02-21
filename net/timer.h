#ifndef TIMER_H
#define TIMER_H

#include <functional>
#include <atomic>
#include "timestamp.h"
#include "util.h"

typedef std::function<void()> TimerCallback;

class Timer : public  Noncopyable {
public:
    Timer(const TimerCallback& cb, TimeStamp expir, double interval): 
        callback_(cb),
        expiration_(expir),
        interval_(interval),
        repeat_(interval > 0.0),
        sequence_(s_numCreated_.fetch_add(1)) { }

    Timer(TimerCallback&& cb, TimeStamp expir, double interval): 
        callback_(cb),
        expiration_(expir),
        interval_(interval),
        repeat_(interval > 0.0),
        sequence_(s_numCreated_.fetch_add(1)) { }

    void Run() const { callback_(); }

    TimeStamp GetExpiration() const { return expiration_; }
    int64_t GetSequence() const { return sequence_; }
    bool IsRepeated() const { return repeat_; }
    void Restart(TimeStamp now);

private:
    const TimerCallback callback_;
    TimeStamp expiration_;
    // 是否可以复用当前Timer
    double interval_;
    bool repeat_;

    const int64_t sequence_;

    static std::atomic<int64_t> s_numCreated_;
};

#endif