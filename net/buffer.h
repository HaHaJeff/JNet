#ifndef BUFFER_H
#define BUFFER_H
#include <algorithm>
#include <string.h>
#include <string>
#include <assert.h>
class Buffer {
  public:
    static const size_t kPrepend = 4;
    Buffer() : buf_(nullptr), b_(kPrepend), e_(kPrepend), cap_(kPrepend), exp_(4096+kPrepend) { }

    ~Buffer() { delete[] buf_; }

    void Clear() {
      delete[] buf_;
      buf_ = nullptr;
      cap_ = 0;
      b_ = e_ = 0;
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

    Buffer& Append(const std::string& str) {
      return Append(str.c_str(), str.size());
    }

    size_t PrependableBytes() {
        return b_;
    }

    void Prepend(const void* data, size_t len) {
        assert(len <= PrependableBytes());
        b_ -= len; 
        const char* d = static_cast<const char*>(data);
        std::copy(d, d+len, buf_+b_);
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
  private:
    char* buf_;
    size_t b_, e_, cap_, exp_;
    void MoveHead() { }
    void Expand(size_t len);
    void CopyFrom(const Buffer& b);
};

#endif
