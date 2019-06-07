#ifndef UTIL_H
#define UTIL_H

#include <string>
#include <functional>
#include <utility>
#include <vector>

#include <string.h>
#include <stdlib.h>

namespace jnet {

// private inherite Noncopyable 
// get the feature of Noncopyable
class Noncopyable {
public:
    Noncopyable() {};
    Noncopyable(const Noncopyable&) = delete;
    Noncopyable& operator=(const Noncopyable&) = delete;
};

// static member function
// call Util::TimeMicro()
class Util {
public:
    static std::string Format(const char* fmt, ...);
    static int64_t TimeMicro();
    static int64_t TimeMilli();
    static int64_t SteadyMicro();
    static int64_t SteadyMilli();
    static std::string ReadableTime(time_t t);
    static int64_t Atoi(const char* b, const char* e);
    static int64_t Atoi(const char* b);
    static int Fcntl(int fd, int flag);
};

class ExitCaller {
public:
    ExitCaller(std::function<void()>&& functor): functor_(std::move(functor)) {}
    ~ExitCaller() { functor_();}
private:
    std::function<void()> functor_;
};
}
#endif
