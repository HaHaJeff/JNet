#ifndef STATUS_H
#define STATUS_H

#include <string>
#include <string.h>
#include <stdarg.h>

#include "util.h"

inline const char* errstr() {return strerror(errno);}

// delete: when argument is nullptr, nothing will to do
class Status {
public:
    Status() : state_(nullptr) { }
    Status(int code, const char* msg);
    Status(int code, const std::string& msg) : Status(code, msg.c_str()) { }
    ~Status() { delete[] state_; }

    Status(const Status& s) { state_ = s.Clone(); }
    void operator=(const Status& s) { delete[] state_; state_ = s.Clone(); }
    Status(Status&& s) { state_ = s.state_; s.state_ = nullptr;} void operator=(Status&& s) { delete[] state_; state_ = s.state_; s.state_ = nullptr; } // errno is threa local
    static Status FromSystem() { return Status(errno, strerror(errno)); }
    static Status FromSystem(int err) { return Status(err, strerror(err));} 
    static Status FromFormat(int code, const char* fmt, ...);
    static Status IoError(const std::string& op, const std::string& name) {
        return Status::FromFormat(errno, "%s %s %s", op.c_str(), name.c_str(), errstr());
    }

    //state_ == nullptr means ok
    int Code() { return state_ ? *(int32_t*)(state_+4) : 0;}
    const char* Msg() { return state_ ? state_ + 8 : "";}
    bool Ok() { return Code() == 0;}
    std::string ToString() { return Util::Format("%d %s", Code(), Msg());}

private:
    // deep copy
    const char* Clone() const;

private:
    // frome leveldb and handy
    // state_[0...3] == length of message
    // state_[4...7] == code   leveldb: state_[4] == code(using enum class)
    // state_[5...]  == message
    const char* state_;
};

#endif