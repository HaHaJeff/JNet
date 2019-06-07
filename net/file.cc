#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "file.h"

namespace jnet {


Status File::GetContent(const std::string& filename, std::string& result) {
    int fd = open(filename.c_str(), O_RDONLY);
    if (fd < 0) {
        return Status::IoError("open", filename);
    }
    // RAII implicit capture
    ExitCaller ec([=]{close(fd);});
    char buf[4096];
    for (;;) {
        int r = read(fd, buf, sizeof(buf));
        if (r < 0) {
            return Status::IoError("read", filename);
        } else if (r == 0) {
            break;
        }
        result.append(buf, r);
    }

    return Status();
}

Status File::WriteContent(const std::string& filename, const std::string& content) {
    int fd = open(filename.c_str(), O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (fd < 0) {
        return Status::IoError("open", filename);
    }
    // RAII implicit capture
    ExitCaller ec([=]{close(fd);});
    int r = write(fd, content.data(), content.size());
    if (r < 0) {
        return Status::IoError("write", filename);
    }
    return Status();
}
}
