#ifndef PROCESSINFO_H
#define PROCESSINFO_H

#include <string>
#include <vector>
#include "timestamp.h"


namespace ProcessInfo {
    pid_t pid();
    std::string pidString();
    uid_t uid();
    std::string userName();
    uid_t euid();
    TimeStamp startTime();
    int clockTicksPerSecond();
    int pageSize();
    bool isDebugBuild();

    std::string hostName();
    std::string procName();

    // read /proc/self/status
    std::string procStatus();
    // read /proc/self/stat
    std::string procStat();
    // read /proc/self/task/tid/stat
    std::string threadStat();
    // read /proc/self/exe
    std::string exePath();

    int openedFiles();
    int maxOpenFiles();

    struct CpuTime {
        double userSeconds;
        double systemSeconds;
        CpuTime() : userSeconds(0.0), systemSeconds(0.0) { }
    };
    CpuTime cpuTime();
    int numThreads();
    std::vector<pid_t> threads();
}

#endif