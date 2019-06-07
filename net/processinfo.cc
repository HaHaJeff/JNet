#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <sys/syscall.h>
#include <sys/times.h>
#include <algorithm>

#include "processinfo.h"
#include "util.h"
#include "file.h"
namespace jnet {


// storage duration
__thread int t_numOpenedFiles = 0;
int FdDirFilter(const struct dirent *d)
{
    if (isdigit(d->d_name[0]))
    {
        ++t_numOpenedFiles;
    }
    return 0;
}

// storage duration
__thread std::vector<pid_t> *t_pids = nullptr;
int TaskDirFilter(const struct dirent *d)
{
    if (isdigit(d->d_name[0]))
    {
        t_pids->push_back(Util::Atoi(d->d_name));
    }
    return 0;
}

int ScanDir(const char *dirpath, int (*filter)(const struct dirent *))
{
    struct dirent **namelist = nullptr;
    // filter: FdDirFilter or TaskDirFilter
    int result = scandir(dirpath, &namelist, filter, alphasort);
    return result;
}

TimeStamp g_startTime = TimeStamp::Now();
int g_clockTicks = static_cast<int>(sysconf(_SC_CLK_TCK));
int g_pageSize = static_cast<int>(sysconf(_SC_PAGE_SIZE));
int g_openMax = static_cast<int>(sysconf(_SC_OPEN_MAX));

pid_t ProcessInfo::pid() { return getpid(); }
std::string ProcessInfo::pidString() { return Util::Format("%ld", ProcessInfo::pid()); }
uid_t ProcessInfo::uid() { return getuid(); }
std::string ProcessInfo::userName()
{
    struct passwd pwd;
    struct passwd *result = nullptr;
    char buf[8192];
    const char *name = "unknowuser";
    getpwuid_r(uid(), &pwd, buf, sizeof(buf), &result);
    if (result)
    {
        name = pwd.pw_name;
    }
    return name;
}

uid_t ProcessInfo::euid() { return geteuid(); }
TimeStamp ProcessInfo::startTime() { return g_startTime; }
int ProcessInfo::clockTicksPerSecond() { return g_clockTicks; }
int ProcessInfo::pageSize() { return g_pageSize; }
bool ProcessInfo::isDebugBuild()
{
#ifdef NDEBUG
    return false;
#else
    return true;
#endif
}

std::string ProcessInfo::hostName()
{
    char buf[256];
    if (gethostname(buf, sizeof(buf)) == 0)
    {
        buf[sizeof(buf) - 1] = '\0';
        return buf;
    }
    else
    {
        return "unknowhost";
    }
}

std::string ProcessInfo::procName()
{
    std::string stat = ProcessInfo::procStat();
    std::string name;
    size_t lp = stat.find('(');
    size_t rp = stat.rfind(')');

    if (lp != std::string::npos && rp != std::string::npos && lp < rp)
    {
        name = std::string(stat.data() + lp + 1, rp - lp - 1);
    }
    return name;
}

std::string ProcessInfo::procStat()
{
    std::string result;
    File::GetContent("/proc/self/stat", result);
    return result;
}

std::string ProcessInfo::procStatus()
{
    std::string result;
    File::GetContent("/proc/self/status", result);
    return result;
}

std::string ProcessInfo::threadStat()
{
    char name[64];
    snprintf(name, sizeof(name), "/proc/self/task/%ld/stat", syscall(__NR_gettid));
    std::string result;
    File::GetContent(name, result);
    return result;
}

std::string ProcessInfo::exePath()
{
    std::string result;
    char buf[1024];
    ssize_t n = readlink("/proc/self/exe", buf, sizeof(buf));
    if (n > 0)
    {
        result.assign(buf, n);
    }
    return result;
}

int ProcessInfo::openedFiles()
{
    t_numOpenedFiles = 0;
    ScanDir("/proc/self/fd", FdDirFilter);
    return t_numOpenedFiles;
}

int ProcessInfo::maxOpenFiles()
{
    return g_openMax;
}

ProcessInfo::CpuTime ProcessInfo::cpuTime()
{
    ProcessInfo::CpuTime t;
    struct tms tms;
    if (times(&tms) >= 0)
    {
        const double hz = static_cast<double>(clockTicksPerSecond());
        t.userSeconds = static_cast<double>(tms.tms_utime)/hz;
        t.systemSeconds = static_cast<double>(tms.tms_stime)/hz;
    }
    return t;
}

int ProcessInfo::numThreads() {
    int result = 0;
    std::string status = ProcessInfo::procStatus();
    size_t pos = status.find("Threads:");
    if (pos != std::string::npos) {
        result = Util::Atoi(status.c_str()+pos+8);
    }
    return result;
}

std::vector<pid_t> ProcessInfo::threads() {
    std::vector<pid_t> result;
    t_pids = &result;
    ScanDir("/proc/self/task", TaskDirFilter);
    t_pids = nullptr;
    std::sort(result.begin(), result.end());
    return result;
}
}
