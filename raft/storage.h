#ifndef STORAGE_H
#define STORAGE_H

#include <memory>
#include <leveldb/db.h>
#include <leveldb/write_batch.h>
#include <leveldb/comparator.h>
#include "raft/state.pb.h"
#include "net/log.h"

namespace jraft {

class Storage {
public:
    Storage(const std::string& path);
    ~Storage();

    void PutCurrentTerm(int64_t currentTerm);
    int64_t GetCurrentTerm();

    void PutVotedFor(int64_t votedFor);
    int64_t GetVotedFor();

    void PutFirstIndex(int64_t firstIndex);
    int64_t GetFirstIndex();

    void PutLastIndex(int64_t lastIndex);
    int64_t GetLastIndex();

    void PutEntry(const LogEntry& entry);
    // 
    // return [firstIndex, lastIndex] entries
    //
    std::vector<LogEntry> GetEntry();

private:
    int64_t currentTerm_;
    int64_t votedFor_;
    int64_t firstIndex_;
    int64_t lastIndex_;

    leveldb::DB* pDb_;
    leveldb::WriteBatch batch_;

    //
    // For int64_t
    void Put(const std::string& key, int64_t value);
    int64_t Get(const std::string& key);
};

}

#endif
