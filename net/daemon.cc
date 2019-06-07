#include "daemon.h"
#include "util.h"

#include <stdio.h>
#include <utility>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>


namespace jnet {

// static function
// only use in current file
static int WritePidFile(const char *pidfile) {
    char str[32];
    int fd = open(pidfile, O_WRONLY | O_CREAT | O_TRUNC, 0664);

    // F_TLOCK lock file, ensure that current file just be wrote by one process
    if (fd < 0 || lockf(fd, F_TLOCK, 0) < 0) {
        fprintf(stderr, "Can't write pid file: %s", pidfile);
        return -1;
    }
    // Use RAII make sure that the file is closed by this process
    ExitCaller call([fd] {close(fd);});

    // write pid to the file
    sprintf(str, "%d\n", getpid());
    ssize_t len = strlen(str);
    ssize_t ret = write(fd, str, len);

    fprintf(stdout, "pid: %s\n", str);

    if (ret != len) {
        fprintf(stderr, "write pidfile %s error!\n", pidfile);
        return -1;
    }
    return 0;
}

int Daemon::GetPidFromFile(const char* pidfile) {
    char buffer[64], *p;
    int fd = open(pidfile, O_RDONLY, 0);

    if (fd  < 0) {
    //    fprintf(stderr, "open pidfile %s error!\n", pidfile);
        return fd;
    }

    ssize_t ret = read(fd, buffer, 64);
    close(fd);

    if (ret <= 0) {
        fprintf(stderr, "read pidfile %s error!\n", pidfile);
        return -1;
    }

    buffer[63] = '\0';
    // Just only need pid (In WritePidFile: "%d\n")
    // strchr(s, c): returns a pointer to the first occurrence of the character c in the string s;
    p = strchr(buffer, '\n');

    if (p == NULL) {
        fprintf(stderr, "content of pidfile %s error!\n", pidfile);
        return -1;
    }

    *p = '\0';
    return Util::Atoi(buffer);
}

int Daemon::DaemonStart(const char* pidfile) {
    int pid = GetPidFromFile(pidfile);

    if (pid > 0) {
        // kill(pid, 0): check the process whether exists.
        // EPERM: doesn't have permission
        if (kill(pid, 0) == 0 || errno == EPERM) {
            fprintf(stderr, "Daemon exists, please use restart!\n");
            return -1;
        }
    }

    // if parent id is 1, the process is already daemon
    if (getppid() == 1) {
        fprintf(stderr, "fork error: %d\n", pid);
        return -1;
    }

    pid = fork();
    // parent exit
    if (pid > 0) {
        exit(0);
    }

    setsid();
    pid = fork();
    if (pid > 0) {
        exit(0);
    }
    int ret = WritePidFile(pidfile);
    if (ret != 0) {
        return ret;
    }

    //redirect 0 1 file description
    //don't redirect 2 file description
    int fd = open("/dev/null", 0);
    if (fd >= 0) {
        dup2(fd, 0);
        dup2(fd, 1);
        close(fd);
        std::string file(pidfile);
        // ensure that the file will be deleted in the end of the process
        static ExitCaller del([file]{
            unlink(file.c_str());
        });

        return 0;
    }
    return -1;
}

int Daemon::DaemonStop(const char* pidfile) {
    int pid = GetPidFromFile(pidfile);
    if (pid <= 0) {
        fprintf(stderr, "%s not exists or not valid\n", pidfile);
        return -1;
    }

    //SIGKILL SIGSTOP can't be ignored and be capture
    int ret = kill(pid, SIGQUIT);
    if (ret < 0) {
        fprintf(stderr, "program %d not exists\n", pid);
        return -1;
    }

    // close twice
    // if the second ret is not zero, close success
    for (int i = 0; i < 1000; ++i) {
        usleep(10*1000);
        ret = kill(pid, SIGQUIT);

        if (ret != 0) {
            fprintf(stderr, "program %d exited\n", pid);
            unlink(pidfile);
            return 0;
        }
    }

    fprintf(stderr, "signal has sended to process, but process still exists after 3 seconds\n");

    return -1;
}

int Daemon::DaemonRestart(const char* pidfile) {
    int pid = GetPidFromFile(pidfile);
    if (pid > 0) {
        if (kill(pid, 0) == 0) {
            int ret = DaemonStop(pidfile);
            if (ret < 0) {
                return ret;
            }
        } else if (errno == EPERM) {
            fprintf(stderr, "don't have permission to kill process: %d\n", pid);
            return -1;
        }
    }
    else {
        fprintf(stderr, "pid file noe valid, just ignore");
    }
    return DaemonStart(pidfile);
}

void Daemon::DaemonProcess(const char* cmd, const char* pidfile) {
    int ret = 0;
    if (cmd == NULL || strcmp(cmd, "start") == 0) {
        ret = DaemonStart(pidfile);
    } else if (strcmp(cmd, "stop") == 0) {
        ret = DaemonStop(pidfile);
        if (ret == 0) {
            exit(0);
        }
    } else if (strcmp(cmd, "restart") == 0) {
        ret = DaemonRestart(pidfile);
    } else {
        fprintf(stderr, "ERROR: bad daemon command. exit\n");
        ret = -1;
    }

    if (ret) {
        exit(1);
    }
}

}
