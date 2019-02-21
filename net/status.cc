#include "status.h"

const char* Status::Clone() const {
    if (state_ == nullptr) {
        return state_;
    }
    uint32_t size = *(uint32_t*)state_;
    char* res = new char[size];
    memcpy(res, state_, size);
    return res;
}

 // state_[0...3] == length of message
 // state_[4...7] == code   leveldb: state_[4] == code(using enum class)
 // state_[5...]  == message
Status::Status(int code, const char* msg) {
    uint32_t sz = strlen(msg) + 8;
    char* p = new char[sz];
    state_ = p;
    // length: include header info
    *(uint32_t*)p = sz;
    // code
    *(uint32_t*)(p+4) = code;
    // msg
    memcpy(p+8, msg, sz-8);
}

// construct Status object from fmt
Status Status::FromFormat(int code, const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    // get size, 1 means '\0'
    uint32_t sz = 8 + vsnprintf(nullptr, 0, fmt, ap) + 1;
    va_end(ap);

    Status res;
    res.state_ = new char[sz];
    char* p = const_cast<char*>(res.state_);
    // length: include header info
    *(uint32_t*)p = sz;
    // code
    *(uint32_t*)(p+4) = code;
    // msg
    va_start(ap, fmt);
    vsnprintf(p+8, sz-8, fmt, ap);
    va_end(ap);
    return res;
}
