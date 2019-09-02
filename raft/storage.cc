#include "storage.h"
#include "log.h"
#include "raft_log.h"

namespace jraft {

const std::string kCurrentTermKey = "currentTerm:";
const std::string kVotedForKey = "votedFor:";
const std::string kFirstIndexKey = "firstIndex:";
const std::string kLastIndexKey = "lastIndex:";

const int64_t kInitialTerm = 0;
const int64_t kVotedFor = -1;
const int64_t kInitialIndex = 0;

Storage::Storage(const std::string& path) : currentTerm_(kInitialTerm),
                                            votedFor_(kVotedFor),
                                            firstIndex_(kInitialIndex),
                                            lastIndex_(kInitialIndex)
{
    leveldb::Options options;
    options.create_if_missing = true;
    leveldb::Status status = leveldb::DB::Open(options, path, &pDb_);
    EXITIF(!status.ok(), "open raft log fail");
}

void Storage::PutCurrentTerm(int64_t currentTerm) {
    if (currentTerm != currentTerm_) {
        currentTerm_ = currentTerm;
        Put(kCurrentTermKey, currentTerm);
    }
}

int64_t Storage::GetCurrentTerm() {
    return currentTerm_;
}

void Storage::PutVotedFor(int64_t votedFor) {
    if (votedFor != votedFor) {
        votedFor_ = votedFor;
        Put(kVotedForKey, votedFor);
    }
}

int64_t Storage::GetVotedFor() {
    return votedFor_;
}

void Storage::PutFirstIndex(int64_t firstIndex) {
    if (firstIndex_ != firstIndex) {
        firstIndex_ = firstIndex;
        Put(kFirstIndexKey, firstIndex);
    }
}

int64_t Storage::GetFirstIndex() {
    return firstIndex_;
}

void Storage::PutLastIndex(int64_t lastIndex) {
    if (lastIndex_ != lastIndex) {
        lastIndex_ = lastIndex;
        Put(kLastIndexKey, lastIndex);
    }
}    

int64_t Storage::GetLastIndex() {
    return lastIndex_;
}

void Storage::PutEntry(const LogEntry& entry) {
    char key[11];
    snprintf(key, sizeof(key), "%010d", entry.index());
    std::string value(entry.SerializeAsString());
    leveldb::Status status = pDb_->Put(leveldb::WriteOptions(), key, value); 
    EXITIF(!status.ok(), "append raft log key = %s, value = %s", key, value.c_str());
}

std::vector<LogEntry> Storage::GetEntry() {
    char firstIndex[11]; char lastIndex[11];
    snprintf(firstIndex, sizeof(firstIndex), "%010d", firstIndex_);
    snprintf(lastIndex, sizeof(lastIndex), "%010d", lastIndex_);

    auto iter = pDb_->NewIterator(leveldb::ReadOptions());
    iter->Seek(firstIndex);

    std::vector<LogEntry> ans;

    for (; iter->Valid(); iter->Next()) {
        auto key = iter->key().ToString();
        if (key > lastIndex) break;
        LogEntry entry;
        std::string str((iter->value()).ToString());
        entry.ParseFromString(str);
        ans.push_back(entry);
    }
    return ans;
}

void Storage::Put(const std::string& key, int64_t value) {
    std::string str(std::to_string(value));
    leveldb::Status status = pDb_->Put(leveldb::WriteOptions(), key, str);
    EXITIF(!status.ok(), "append raft log key = %s, value = %s", key.c_str(), str.c_str());
}

int64_t Storage::Get(const std::string& key) {
    std::string value;
    leveldb::Status status = pDb_->Get(leveldb::ReadOptions(), key, &value);
    EXITIF(!status.ok(), "read raft log key = %s, value = %s", key.c_str(), value.c_str());
    return strtoull(value.c_str(), 0, 0);
}

Storage::~Storage() {
    if (pDb_ != nullptr) delete pDb_;
    pDb_ = nullptr;
    INFO("close raft log!");
}

}
