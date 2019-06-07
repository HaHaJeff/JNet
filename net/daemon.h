#ifndef DAEMON_H
#define DAEMON_H

#include <functional>
#include <signal.h>

namespace jnet {

class Daemon {
public:
    static int DaemonStart(const char* pidfile);
    static int DaemonRestart(const char* pidifle);
    static int DaemonStop(const char* pidfile);
    static int GetPidFromFile(const char* pidfile);
    // cmd: start stop restart
    static void DaemonProcess(const char* cmd, const char* pidfile);
    static void ChangeTo(const char* argv[]);
};

struct SignalHandler{
    static void Signal(int sig, const std::function<void()>& handler);
};

}
#endif
