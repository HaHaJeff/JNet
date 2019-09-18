#ifndef STORAGE_H
#define STORAGE_H

#include <string>

namespace jraft {

class Storage {
public:
    Storage(const std::string& path);
    int Write(const char* buf, int len, off64_t off);
    int Read(char* buf, int len, off64_t off);
    int64_t FileSize();
    ~Storage();
private:
    int fd_;
};

}

#endif
