#ifndef TIMERID_H
#define TIMERID_H

#include <stdint.h>

class Timer;

class TimerId {
public:
    TimerId() : timer_(nullptr), sequence_(0) { }
    TimerId(Timer* timer, int64_t id) : timer_(timer), sequence_(id) { }

    friend class TimerQueue;
private:
    Timer* timer_;
    int64_t sequence_;
};

#endif