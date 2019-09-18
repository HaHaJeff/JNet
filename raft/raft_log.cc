#include "raft_log.h"

namespace jraft
{

const std::string id_header = "LOG_ID:";
const int id_len = id_header.length();
const std::string cmd_header = "COMMAND:";
const int cmd_len = cmd_header.length();

std::string LogId::to_string()
{
    return id_header + std::to_string(index_) + ":" + std::to_string(term_) + ":";
}

void LogId::from_string(const std::string &str)
{
    int pos_id = str.find(id_header);

    int pos_index = str.find(":", pos_id + id_len + 1);
    std::string str_index(str.substr(pos_id + 1, pos_index));
    index_ = std::stoi(str_index);

    int pos_term = str.find(":", pos_index + 1);
    std::string str_term(str.substr(pos_id + 1, pos_term));
    term_ = std::stoi(str_term);
}

std::string Command::to_string()
{
    std::string ret;
    switch (type_)
    {
    case CONFIG_CHANGE:
        ret = "CONFIG_CHANGE:";
        break;
    case APPEND_ENTRY:
        ret = "APPEND_ENTRY:";
        break;
    }
    return cmd_header + ret + data_;
}

void Command::from_string(const std::string &str)
{
    int pos_cmd = str.find(cmd_header);

    int pos_type = str.find(":", pos_cmd + cmd_len + 1);
    std::string str_type(str.substr(pos_type, pos_type + 1));
    if (str_type == "CONFIG_CHANGE:")
    {
        type_ = CONFIG_CHANGE;
    }
    else if (str_type == "APPEND_ENTRY:")
    {
        type_ = APPEND_ENTRY;
    }
    data_ = str.substr(pos_type + 1);
}

std::string LogEntry1::to_string()
{
    return id_.to_string() + cmd_.to_string();
}

void LogEntry1::from_string(const std::string &str)
{
    id_.from_string(str), cmd_.from_string(str);
}

RaftLog::RaftLog(Storage *storage) : firstIndex_(-1),
                                     storage_(storage)
{
}

void RaftLog::Append(const LogEntry& log)
{
    
}

void RaftLog::Append(const std::vector<LogEntry>& logs)
{

}

void RaftLog::Load()
{

}

LogEntry* RaftLog::Get(int64_t index)
{
    return nullptr;
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

bool RaftLog::ValidIndex(int64_t index) const
{
    return !offset_term_.empty() || (index - firstIndex_) > offset_term_.size()-1;
}

} // namespace jraft
