#include "timer.h"

std::atomic<int64_t> Timer::s_numCreated_;

void Timer::Restart(TimeStamp now) {
    if (repeat_) {
        expiration_ = AddTime(now, interval_);
    }
    else {
        expiration_ = TimeStamp();
    }
}
