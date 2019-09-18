#include "storage.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <cassert>

namespace jraft {
    
Storage::Storage(const std::string& path)
{
    fd_ = open(path.c_str(), O_RDWR | O_SYNC | O_CREAT, 0666);
    assert(fd_ != -1);
}

int Storage::Write(const char* buf, int len, off64_t off)
{
    return pwrite(fd_, buf, len, off);
}

int Storage::Read(char* buf, int len, off64_t off)
{
    return pread(fd_, buf, len, off);
}

int64_t Storage::FileSize()
{
    struct stat stat_buf;
    fstat(fd_, &stat_buf);
    return stat_buf.st_size;
}

Storage::Storage::~Storage() {
    close(fd_);
}

}
