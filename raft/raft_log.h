#ifndef RAFT_LOG_H
#define RAFT_LOG_H

//
// for shared_ptr and unique_ptr
//
#include <memory>

#include "state.pb.h"
namespace jraft {

class Storage;

class RaftLog {
public:
    RaftLog(Storage* storage) : firstIndex_(-1),
                                lastIndex_(-1),
                                storage_(storage)
    {
    }
    void SetFirstIndex(int index) { firstIndex_ = index; } 
    void SetLastIndex(int index) { lastIndex_ = index; }
private:
    int64_t firstIndex_;
    int64_t lastIndex_;
    Storage* storage_;
    //
    // type: noop, data, configuration
    // term
    // index
    // command
    //
    std::vector<LogEntry> entries_;
};

struct LogId {
    LogId() : index_(0), term_(0) {}
    LogId(int64_t index, int64_t term) : index_(index), term_(term) {}
    int64_t index_;
    int64_t term_;
    std::string to_string();
    void from_string(const std::string&);
};

enum TYPE {
    CONFIG_CHANGE=0,
    APPEND_ENTRY=1
};

struct Command {
    Command(TYPE type, const std::string& data) : type_(type), data_(data) {}
    Command(){}
    TYPE type_;
    std::string data_;
    std::string to_string();
    void from_string(const std::string&);
};

struct LogEntry1 {
    LogEntry1(const LogId& id, const Command& cmd) : id_(id), cmd_(cmd) {}
    LogEntry1() {}
    LogId id_;
    Command cmd_;
    std::string to_string();
    void from_string(const std::string&);
};

}
#endif
