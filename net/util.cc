#include "util.h"
#include <chrono>
#include <memory>
#include <stdarg.h>
#include <unistd.h>
#include <fcntl.h>

std::string Util::Format(const char *fmt, ...)
{

    char tmpBuffer[200];
    char *base = tmpBuffer;
    int buffsize = sizeof(tmpBuffer);

    va_list vl;
    va_start(vl, fmt);

    int ret = vsnprintf(base, buffsize, fmt, vl);

    //unique_ptr will call delete[] when template is array
    std::unique_ptr<char[]> release;
    if (ret >= buffsize)
    {
        buffsize = 10000;
        base = new char[buffsize];
        release.reset(base);
        ret = vsnprintf(base, buffsize, fmt, vl);
    }

    va_end(vl);

    // if still not enough, then truncate
    if (ret >= buffsize)
    {
        *(base + buffsize - 1) = '\0';
    }
    //string construct will copy content of tmpBuffer
    return base;
}

int64_t Util::TimeMicro()
{
    auto time = std::chrono::system_clock::now();
    return std::chrono::duration_cast<std::chrono::microseconds>(time.time_since_epoch()).count();
}

int64_t Util::TimeMilli()
{
    return TimeMicro() / 1000;
}

int64_t Util::SteadyMicro()
{
    auto time = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::microseconds>(time.time_since_epoch()).count();
}

int64_t Util::SteadyMilli()
{
    return SteadyMilli() / 1000;
}

/*
struct tm
{
    int tm_sec;   // Seconds (0-60)
    int tm_min;   // Minutes (0-59)
    int tm_hour;  // Hours (0-23)
    int tm_mday;  // Day of the month (1-31)
    int tm_mon;   // Month (0-11)
    int tm_year;  // Year - 1900
    int tm_wday;  // Day of the week (0-6, Sunday = 0)
    int tm_yday;  // Day in the year (0-365, 1 Jan = 0)
    int tm_isdst; // Daylight saving time
};
*/
std::string Util::ReadableTime(time_t t)
{
    struct tm tm1;
    localtime_r(&t, &tm1);
    return (Format("%04d-%02d-%02d %02d:%02d:%02d", tm1.tm_year + 1900, tm1.tm_mon + 1, tm1.tm_mday, tm1.tm_hour, tm1.tm_min, tm1.tm_sec));
}

int64_t Util::Atoi(const char *b, const char *e)
{
    return strtol(b, (char**)&e, 10);
}

int64_t Util::Atoi(const char *b)
{
    return Atoi(b, b+strlen(b));
}

int Util::Fcntl(int fd, int flag)
{
    int ret = fcntl(fd, F_GETFD);
}