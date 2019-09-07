#ifndef LOG_H
#define LOG_H

#include <algorithm>
#include <atomic>
#include "util.h"

// thanks to handy
#define hlog(level, ...)  {                                                                 \
    do                                                                                      \
    {                                                                                       \
        if (level <= Logger::GetLogger().GetLogLevel())                                     \
        {                                                                                   \
            Logger::GetLogger().Write(level, __FILE__, __LINE__, __func__, __VA_ARGS__);    \
        }                                                                                   \
    }                                                                                       \
    while(0);                                                                               \
}

#define TRACE(...) hlog(Logger::LTRACE, __VA_ARGS__)
#define DEBUG(...) hlog(Logger::LDEBUG, __VA_ARGS__)
#define INFO(...)  hlog(Logger::LINFO, __VA_ARGS__)
#define WARN(...)  hlog(Logger::LWARN, __VA_ARGS__)
#define ERROR(...) hlog(Logger::LERROR, __VA_ARGS__)
#define FATAL(...) hlog(Logger::LFATAL, __VA_ARGS__)
#define FATALIF(b, ...) do {if (b) { hlog(Logger::LFATAL, __VA_ARGS__);} } while(0)
#define EXITIF(b, ...)do {if (b) { hlog(Logger::LERROR, __VA_ARGS__); exit(1);} } while(0)

#define SETLOGLEVEL(level) Logger::GetLogger().SetLogLevel(level)
#define SETLOGFILE(name) Logger::GetLogger().SetFileName(name)

class Logger : private Noncopyable {
public:
    enum LogLevel{LFATAL=0, LERROR, LWARN, LINFO, LDEBUG, LTRACE, LALL};
    Logger();
    ~Logger();
    void Write(int level, const char* file, int line, const char* func, const char* fmt,...);

    void SetLogLevel(const std::string& level);
    void SetFileName(const std::string& name);
    void SetLogLevel(LogLevel level) { level_ = std::min(LALL, std::max(LFATAL, level));}

    LogLevel GetLogLevel() const { return level_; }
    const char* GetLogLevelStr() { return levelStrs_[level_]; }
    int GetFd() { return fd_; }

    void AdjustLogLevel(int adjust) { SETLOGLEVEL(LogLevel(level_ + adjust)); }
    void SetRotateInterval(long rotateInterval) { rotateInterval_ = rotateInterval; }
    static Logger& GetLogger();

private:
    void MaybeRotate();
    static const char* levelStrs_[LALL+1];
    int fd_;
    LogLevel level_;
    long lastRotate_;
    std::atomic<int64_t> realRotate_;
    long rotateInterval_;
    std::string filename_;
};
#endif
