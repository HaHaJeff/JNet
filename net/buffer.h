#ifndef BUFFER_H
#define BUFFER_H
#include <algorithm>
#include <string.h>
#include <string>
#include <assert.h>
#include <iostream>
#include <arpa/inet.h>
class Buffer {
  public:
    Buffer() : buf_(nullptr), b_(0), e_(0), cap_(0), exp_(4096), extra_(0) { }

    ~Buffer() { delete[] buf_; }

    void Clear() {
      delete[] buf_;
      buf_ = nullptr;
      cap_ = 0;
      b_ = e_ = 0;
    }

    const char* Peek() const {
        return Begin();
    }

    int32_t PeekInt32() const { 
        int32_t be32 = 0;
        ::memcpy(&be32, Begin(), sizeof be32);
        return be32;
    }

    size_t GetSize() const { return e_ - b_; }

    bool IsEmpty() const { return e_ == b_; }

    char* GetData() const { return buf_ + b_; }

    char* Begin() const { return buf_ + b_; }

    char* End() const { return buf_ + e_; }

    char* MakeRoom(size_t len);

    void MakeRoom() {
      if ( GetSpace() < exp_)
        Expand(0);
    }

    size_t GetSpace() const { return cap_ - e_; }

    void AddSize(size_t len) { e_ += len; }

    char* AllocRoom(size_t len) {
      char* p = MakeRoom(len);
      AddSize(len);
      return p;
    }

    const char* Peek() {
        return Begin();
    }

    int32_t PeekInt32() {
        assert(GetSize()>= sizeof(int32_t));
        int32_t be32 = 0;
        ::memcpy(&be32, Peek(), sizeof be32);
        return ntohl(be32);
    }

    void AppendInt32(int32_t num) {
        int32_t be32 = htonl(num);
        Append(reinterpret_cast<char*>(&be32), sizeof be32);
    }

    Buffer& Append(const std::string& str) {
      return Append(str.c_str(), str.size());
    }

    void EnsureWriteableBytes(size_t len) {
        if (GetSize() < len) {
            MakeRoom(len);
        }
    }

    Buffer& Append(const char* p, size_t len) {
      memcpy(AllocRoom(len), p, len);
      return *this;
    }

    Buffer& Append(const char*p) {
      return Append(p, strlen(p));
    }

    template <class T>
    Buffer& AppendValue(const T& v) {
      Append((const char*)&v, sizeof(v));
      return *this;
    }

    Buffer& Consume(size_t len) {
      b_ += len;
      if (GetSize() == 0) Clear();
      return *this;
    }

    Buffer& Absorb(Buffer& buf);
    void SetSuggestSize(size_t sz) { exp_ = sz; }

    Buffer& operator=(const Buffer& b) {
      if (this == &b) return *this;
      delete[] buf_;
      buf_ = nullptr;
      CopyFrom(b);
      return *this;
    }
    int extra_;
  private:
    char* buf_;
    size_t b_, e_, cap_, exp_;
    void MoveHead() { }
    void Expand(size_t len);
    void CopyFrom(const Buffer& b);
};

#endif
