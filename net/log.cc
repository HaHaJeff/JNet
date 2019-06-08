#include <assert.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <fcntl.h>
#include <time.h>
#include <utility>
#include <stdarg.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <syslog.h>

#include "net/log.h"
#include "timestamp.h"

namespace jnet {

Logger::Logger() : level_(LALL), lastRotate_(time(NULL)), rotateInterval_(86400) {
    tzset();
    fd_ = -1;
    realRotate_ = lastRotate_;
}

Logger::~Logger() {
    if (fd_ != -1) {
        close(fd_);
    }
}

const char* Logger::levelStrs_[LALL+1] = {
    "FATAL",
    "ERROR",
    "WARN",
    "INFO",
    "DEBUG",
    "TRAVE",
    "ALL",
};

Logger& Logger::GetLogger() {
    static Logger logger;
    return logger;
}

void Logger::SetLogLevel(const std::string& level) {
    LogLevel iLevel = LINFO;
    for (size_t i = 0; i < sizeof(levelStrs_)/sizeof(const char*); i++) {
        if (strcasecmp(levelStrs_[i], level.c_str()) == 0) {
            iLevel = static_cast<LogLevel>(i);
            break;
        }
    }
    SETLOGLEVEL(iLevel);
}

void Logger::SetFileName(const std::string& filename) {
  int fd = open(filename.c_str(), O_APPEND|O_CREAT|O_WRONLY|O_CLOEXEC, DEFFILEMODE);
  if (fd < 0) {
      fprintf(stderr, "open log file %s failed. msg: %s ignored\n",
              filename.c_str(), strerror(errno));
      return;
  }
  filename_ = filename;
  if (fd_ == -1) {
      fd_ = fd;
  } else {
      int r = dup2(fd, fd_);
      FATALIF(r<0, "dup2 failed");
      close(fd);
    }
}

void Logger::MaybeRotate() {
    time_t now = time(nullptr);
    // timezone: tzset call in Logger
    // 如果文件不存在或者已经在rotateInterval_时间间隔内更新过
    if (filename_.empty() || (now - timezone)/rotateInterval_ == (lastRotate_ - timezone)/rotateInterval_) {
        return;
    }
    lastRotate_ = now;
    // exchange返回realRotate_的旧值，并更新realRotate_的值为now
    long old = realRotate_.exchange(now);

    if ((old - timezone)/rotateInterval_ == (lastRotate_ - timezone)/rotateInterval_) {
        return;
    }

    TimeStamp t(TimeStamp::Now());
    std::string newname = filename_ + t.ToFormattedString();

    int err = rename(filename_.c_str(), newname.c_str());
    if (err != 0) {
        fprintf(stderr, "rename logfile %s -> %s failed msg: %s\n", filename_.c_str(), newname.c_str(), strerror(errno));
        return;
    }
    int fd = open(filename_.c_str(), O_APPEND | O_CREAT | O_WRONLY | O_CLOEXEC, DEFFILEMODE);

    if (fd < 0) {
       fprintf(stderr, "open log file %s failed. msg: %s ignored\n", newname.c_str(), strerror(errno));
       return;
    }

    dup2(fd, fd_);
    close(fd);
}


//TODO: support multithread
static thread_local uint64_t tid;
void Logger::Write(int level, const char* file, int line, const char*func, const char* fmt, ...) {
    if (level > level_) {
        return;
    }

    MaybeRotate();
    char buffer[4*1024];
    char* p = buffer;
    char* limit = buffer+sizeof(buffer);

    TimeStamp t(TimeStamp::Now());
    std::string tm = t.ToFormattedString() + " ";
    memcpy(p, tm.c_str(), tm.size());
    p += tm.size();

    va_list args;
    va_start(args, fmt);
    p += vsnprintf(p, limit-p, fmt, args);
    va_end(args);

    p = std::min(p, limit-2);
    // trim the ending \n
    while (*--p == '\n') {
    }
    *++p = '\n';
    *++p = '\0';

    int fd = fd_ == -1 ? 1 : fd_;

    int err = ::write(fd, buffer, p-buffer);
    if (err != p - buffer) {
        fprintf(stderr, "write log file %s failed. writted %d errmsg: %s\n", filename_.c_str(), err, strerror(errno));
    }

    if (level <= LERROR) {
        syslog(LOG_ERR, "%s", buffer+27);
    }
    if (level == LFATAL) {
        fprintf(stderr, "%s", buffer);
        //assert(0);
    }
}
}
