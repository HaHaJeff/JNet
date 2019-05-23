#ifndef TIMESTAMP_H
#define TIMESTAMP_H

#include <string>
#include "util.h"

//from muduo
class TimeStamp {
public:
    TimeStamp() : microSecondsSinceEpoch_(0) { }
    explicit TimeStamp(int64_t microSecondsSinceEpoch) : microSecondsSinceEpoch_(microSecondsSinceEpoch) { }
    void Swap(TimeStamp& that) { std::swap(microSecondsSinceEpoch_, that.microSecondsSinceEpoch_); }
    std::string ToString() const;
    std::string ToFormattedString(bool showMicroSeconds = true) const;
    bool Valid() const { return microSecondsSinceEpoch_ != 0; }
    int64_t MicroSecondsSinceEpoch() const { return microSecondsSinceEpoch_; }
    time_t SecondsSinceEpoch() const { return static_cast<time_t>(microSecondsSinceEpoch_/kMicroSecondsPerSecond); }
    static TimeStamp Now();
    static TimeStamp InValid() { return TimeStamp(); }
    static TimeStamp FromUnixTime(time_t t) {return FromUnixTime(t, 0); }
    static TimeStamp FromUnixTime(time_t t, int64_t microSeconds) { return TimeStamp(static_cast<int64_t>(t)*kMicroSecondsPerSecond + microSeconds); }

    static const int kMicroSecondsPerSecond = 1000*1000;

private:
    int64_t microSecondsSinceEpoch_;
};

// overload operator < ==
inline bool operator<(TimeStamp lhs, TimeStamp rhs) {
    return lhs.MicroSecondsSinceEpoch() < rhs.MicroSecondsSinceEpoch();
}

inline bool operator==(TimeStamp lhs, TimeStamp rhs) {
    return lhs.MicroSecondsSinceEpoch() == rhs.MicroSecondsSinceEpoch();
}

// rhs > lhs
inline double TimeDifference(TimeStamp lhs, TimeStamp rhs) {
    int64_t diff = rhs.MicroSecondsSinceEpoch() - lhs.MicroSecondsSinceEpoch();
    return static_cast<double>(diff)/TimeStamp::kMicroSecondsPerSecond;
}

inline TimeStamp AddTime(TimeStamp t, double seconds) {
    int64_t delta = static_cast<int64_t>(seconds*TimeStamp::kMicroSecondsPerSecond);
    TimeStamp ret(t.MicroSecondsSinceEpoch()+delta);
    return ret;
}

#endif
