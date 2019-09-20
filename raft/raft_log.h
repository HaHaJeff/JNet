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
    // construct offset_term_
    void Load();
    LogEntry* Get(int64_t index);
    int64_t Term(int64_t index) const;
    int64_t FirstIndex() const;
    int64_t LastIndex() const;
    void SetFirstIndex(int64_t index);
    void Truncate(const int64_t& index);
    bool ContainLog(int64_t index, int64_t term);
private:
    // index - firstIndex = pos of offset_term_
    void GetMeta(int64_t index, LogMeta &meta);
    bool ValidIndex(int64_t index) const;

private:
    int64_t firstIndex_;
    int64_t bytes_;
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


} // namespace jraft
#endif
