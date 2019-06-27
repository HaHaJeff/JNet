#ifndef STORAGE_H
#define STORAGE_H

#include <memory>
#include <leveldb/db.h>
#include "net/log.h"

namespace jraft {

class Storage {
public:
    Storage(const std::string& path);
    ~Storage();
    void SavePersist(const std::string& input);
    std::string ReadPersist();
private:
    leveldb::DB* pDb_;
    const std::string key_ = "leader";
};

}

#endif
