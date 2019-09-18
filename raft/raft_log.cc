#include "raft_log.h"
#include "storage.h"
#include "log.h"

namespace jraft {

struct LogHeader
{
    LogHeader(int64_t term, int len) : term_(term), len_(len) {}
    LogHeader() : term_(0), len_(0) {}
    int64_t term_;
    int len_;
};

RaftLog::RaftLog(Storage *storage) : firstIndex_(-1),
                                     bytes_(0),
                                     storage_(storage)
{
}

void RaftLog::Append(const LogEntry& log)
{
    int64_t term = log.term();
    std::string str = log.SerializeAsString();
    int len = str.size();
    LogHeader header(term, len);
    if (-1 == storage_->Write(reinterpret_cast<char*>(&header), sizeof(header), bytes_))
    {
        ERROR("RaftLog append write header error!");
        return;
    }
    if (-1 == storage_->Write(str.c_str(), len, bytes_+sizeof(header)))
    {
        ERROR("RaftLog append write content error!");
        return;
    }
    offset_term_.push_back({bytes_, term});
    bytes_ += len + sizeof(header);
}

void RaftLog::Load()
{
    off64_t off = 0;
    int64_t file_size = storage_->FileSize();
    if (-1 == storage_->Read(reinterpret_cast<char*>(&firstIndex_), sizeof(firstIndex_), off))
    {
        ERROR("RaftLog load read firstIndex error!");
        return;
    }
    off += 8;
    while (off < file_size)
    {
        LogHeader header;
        if (-1 == storage_->Read(reinterpret_cast<char*>(&header), sizeof(header), off))
        {
            ERROR("RaftLog load read header error!");
            return;
        }
        offset_term_.push_back({off, header.term_});
        char buf[header.len_];
        if (-1 == storage_->Read(buf, sizeof(buf), off+sizeof(header)))
        {
            ERROR("RaftLog load read conntent error!");
            return;
        }
        off += sizeof(header) + header.len_;
    }
}

LogEntry* RaftLog::Get(int64_t index)
{
    LogHeader header;
    if (ValidIndex(index) == false) return nullptr;
    off64_t off = offset_term_[index].first;
    if (-1 == storage_->Read(reinterpret_cast<char*>(&header), sizeof(header), off))
    {
        ERROR("RaftLog get header of index=%lld error", index);
        return nullptr;         
    }
    std::cout << header.term_ << std::endl;
    std::cout << header.len_ << std::endl;
    int len = header.len_;
    char buf[len];
    if (-1 == storage_->Read(buf, sizeof(buf), off+sizeof(header)))
    {
        ERROR("RaftLog get content of index=%lld error", index);
        return nullptr;         
    }
    LogEntry* entry = new LogEntry;
    entry->ParseFromString(buf);
    return entry;
}

int64_t RaftLog::Term(int64_t index) const
{
    if (ValidIndex(index)) return 0;
    return offset_term_[index-firstIndex_].second;
}

int64_t RaftLog::FirstIndex() const
{
    return firstIndex_;
}

int64_t RaftLog::LastIndex() const
{
    return offset_term_.size() + firstIndex_ - 1;
}

void RaftLog::SetFirstIndex(int64_t index)
{
    if (-1 == storage_->Write(reinterpret_cast<char*>(&firstIndex_), sizeof(firstIndex_), bytes_))
    {
        ERROR("Raft SetFirstIndex index = %lld error!", index);
        return;
    }
    firstIndex_ = index;
}

bool RaftLog::ValidIndex(int64_t index) const
{
    return !offset_term_.empty() && (index - firstIndex_) >= offset_term_.size()-1;
}

} // namespace jraft
