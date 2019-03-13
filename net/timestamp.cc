// For Print int64 "%PRId64"
#include <inttypes.h>
#include <time.h>
#include <sys/time.h>

#include "timestamp.h"

std::string TimeStamp::ToString() const {
    char buf[32] = {0};
    int64_t seconds = microSecondsSinceEpoch_/kMicroSecondsPerSecond;
    int64_t microseconds = microSecondsSinceEpoch_%kMicroSecondsPerSecond;

    //"%.06" reserve six bit, pad with zero
    snprintf(buf, sizeof(buf-1), "%" PRId64 "%.0" PRId64 "", seconds, microseconds);
    return buf;
}

std::string TimeStamp::ToFormattedString(bool showMicroSeconds) const {
    char buf[32] = {0};
    time_t seconds = static_cast<time_t>(microSecondsSinceEpoch_/kMicroSecondsPerSecond);
    struct tm tm_time;
    gmtime_r(&seconds, &tm_time); 

    if (showMicroSeconds) {
        int microSeconds = static_cast<int>(microSecondsSinceEpoch_%kMicroSecondsPerSecond);
        snprintf(buf, sizeof(buf)-1, "%04d-%02d-%02d %02d:%02d:%02d.%06d",
            tm_time.tm_year + 1900, tm_time.tm_mon+1, tm_time.tm_mday,
            tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec, microSeconds);
    } else {
        snprintf(buf, sizeof(buf)-1, "%04d-%02d-%02d %02d:%02d:%02d",
            tm_time.tm_year+1900, tm_time.tm_mon+1, tm_time.tm_mday,
            tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);
    }
    return buf;
}

TimeStamp TimeStamp::Now() {
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    int64_t seconds = tv.tv_sec;
    return TimeStamp(seconds*kMicroSecondsPerSecond + tv.tv_usec);
}
