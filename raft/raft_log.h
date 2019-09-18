#ifndef RAFT_LOG_H
#define RAFT_LOG_H

//
// for shared_ptr and unique_ptr
//
#include <memory>
#include <sys/types.h>

#include "state.pb.h"
namespace jraft
{

class Storage;

struct LogMeta
{
    off_t offset_;
    size_t length_;
    int64_t term_;
};

// header
// firstIndex 8 byte

// log format
// | ------- term (64bits) ---------|
// | --------data len (32bits)------|
class RaftLog
{
public:
    RaftLog(Storage *storage);
    void Append(const LogEntry &log);
    void Append(const std::vector<LogEntry> &logs);
    void Load();
    LogEntry* Get(int64_t index);
    int64_t Term(int64_t index) const;
    int64_t FirstIndex() const;
    int64_t LastIndex() const;

private:
    // index - firstIndex = pos of offset_term_
    void GetMeta(int64_t index, LogMeta &meta);
    bool ValidIndex(int64_t index) const;

private:
    int64_t firstIndex_;
    // offset, term, convenient to get index and term
    std::vector<std::pair<off_t, int64_t>> offset_term_;
    Storage *storage_;
    //
    // type: noop, data, configuration
    // term
    // index
    // command
    //
    std::vector<LogEntry> entries_;
};

struct LogId
{
    LogId() : index_(0), term_(0) {}
    LogId(int64_t index, int64_t term) : index_(index), term_(term) {}
    int64_t index_;
    int64_t term_;
    std::string to_string();
    void from_string(const std::string &);
};

enum TYPE
{
    CONFIG_CHANGE = 0,
    APPEND_ENTRY = 1
};

struct Command
{
    Command(TYPE type, const std::string &data) : type_(type), data_(data) {}
    Command() {}
    TYPE type_;
    std::string data_;
    std::string to_string();
    void from_string(const std::string &);
};

struct LogEntry1
{
    LogEntry1(const LogId &id, const Command &cmd) : id_(id), cmd_(cmd) {}
    LogEntry1() {}
    LogId id_;
    Command cmd_;
    std::string to_string();
    void from_string(const std::string &);
};

} // namespace jraft
#endif
