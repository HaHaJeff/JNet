#include "raft_log.h"
#include "storage.h"
#include "log.h"

namespace jraft {

struct LogHeader
{
    LogHeader(int64_t term, int len) : term_(term), len_(len) {}
    LogHeader() : term_(0), len_(0) {}
    void Print() { INFO("term: %lld, len: %lld", term_, len_);}
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
    int64_t file_size = storage_->FileSize();
    if (-1 == storage_->Read(reinterpret_cast<char*>(&firstIndex_), sizeof(firstIndex_), bytes_))
    {
        ERROR("RaftLog load read firstIndex error!");
        return;
    }
    bytes_ += 8;
    while (bytes_ < file_size)
    {
        LogHeader header;
        if (-1 == storage_->Read(reinterpret_cast<char*>(&header), sizeof(header), bytes_))
        {
            ERROR("RaftLog load read header error!");
            return;
        }
        offset_term_.push_back({bytes_, header.term_});
        char buf[header.len_];
        if (-1 == storage_->Read(buf, sizeof(buf), bytes_+sizeof(header)))
        {
            ERROR("RaftLog load read conntent error!");
            return;
        }
        bytes_ += sizeof(header) + header.len_;
    }
}

LogEntry* RaftLog::Get(int64_t index)
{
    LogHeader header;
    if (ValidIndex(index) == false) return nullptr;
    off64_t off = offset_term_[index-firstIndex_].first;
    if (-1 == storage_->Read(reinterpret_cast<char*>(&header), sizeof(header), off))
    {
        ERROR("RaftLog get header of index=%lld error", index);
        return nullptr;         
    }
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
    if (-1 == storage_->Write(reinterpret_cast<char*>(&index), sizeof(index), bytes_))
    {
        ERROR("Raft SetFirstIndex index = %lld error!", index);
        return;
    }
    bytes_ += sizeof(firstIndex_);
    firstIndex_ = index;
}

void RaftLog::Truncate(const int64_t& index)
{
    if (!ValidIndex(index)) return;
    if (index <= firstIndex_) 
    {
        assert(-1 != storage_->Truncate(0)); 
        bytes_ = 0;
        SetFirstIndex(index);
    }
    assert(-1 != storage_->Truncate(offset_term_[index-firstIndex_].first)); 
    bytes_ = offset_term_[index-firstIndex_].first;
    offset_term_.erase(offset_term_.begin()+index-firstIndex_);
}

bool RaftLog::ContainLog(int64_t index, int64_t term)
{
    if (!ValidIndex(index)) return false;
    return offset_term_[index-firstIndex_].second == term;
}

bool RaftLog::ValidIndex(int64_t index) const
{
    return !offset_term_.empty() && (index - firstIndex_) < offset_term_.size();
}

} // namespace jraft
