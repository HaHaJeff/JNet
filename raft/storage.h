#ifndef STORAGE_H
#define STORAGE_H

#include <memory>
#include <leveldb/db.h>
#include "net/log.h"

namespace jraft {

class Storage {
public:
    Storage(const std::string& path);
    void append(int64_t key, const std::string& value);
    std::string load(int64_t key);
    ~Storage();
private:
    leveldb::DB* pDb_;
};

}

#endif
