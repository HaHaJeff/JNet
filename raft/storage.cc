#include "storage.h"
#include "net/log.h"
#include "raft/log.h"

using namespace jnet;

namespace jraft {

Storage::Storage(const std::string& path) {
    leveldb::Options options;
    options.create_if_missing = true;
    leveldb::Status status = leveldb::DB::Open(options, path, &pDb_);
    EXITIF(!status.ok(), "open raft log fail");
}

/*
void Storage::append(int64_t key, const std::string& value) {
    leveldb::Status status = pDb_->Put(leveldb::WriteOptions(), std::to_string(key), value);
    EXITIF(!status.ok(), "append raft log key = %lld, value = %s", key, value.c_str());
}

std::string Storage::load(int64_t key) {
    std::string value;
    leveldb::Status status = pDb_->Get(leveldb::ReadOptions(), std::to_string(key), &value);
    EXITIF(!status.ok(), "load raft log key = %lld, value = %s", key, value.c_str());
    return value;
}
*/

void Storage::SavePersist(const std::string& input) {
    leveldb::Status status = pDb_->Put(leveldb::WriteOptions(), key_, input);
    EXITIF(!status.ok(), "append raft log key = %s, value = %s", key_.c_str(), input.c_str());
}

std::string Storage::ReadPersist() {
    std::string value;
    leveldb::Status status = pDb_->Get(leveldb::ReadOptions(), key_, &value);
    EXITIF(!status.ok(), "load raft log key = %s, value = %s", key_.c_str(), value.c_str());
    return value;
}

Storage::~Storage() {
    if (pDb_ != nullptr) delete pDb_;
    pDb_ = nullptr;
    INFO("close raft log!");
}

}
