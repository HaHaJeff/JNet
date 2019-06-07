#include "buffer.h"

namespace jnet {

char* Buffer::MakeRoom(size_t len) {
 
  //如果足够容纳，这什么都不做
  if (e_ + len <= cap_) {
    // nothing to do
  } else if (GetSize() + len < cap_) {
    // 如果在b_之前与e_之后的空间之和可以容纳，则移动
    MoveHead();
  } else {
    Expand(len);
  }
  return End();
}

// 以一定的比扩大buffer
void Buffer::Expand(size_t len) {
  size_t ncap = std::max(exp_, std::max(2*cap_, GetSize() + len));
  char* p = new char[ncap];
  std::copy(Begin(), End(), p);
  e_ -= b_;
  b_ = 0;
  delete[] buf_;
  buf_ = p;
  cap_ = ncap;
}

void Buffer::CopyFrom(const Buffer& b) {
  // buf_ = b.buf_;
  // b_ = b.b_;
  // e_ = b.e_;
  // cap_ = b.cap_;
  // exp_ = b.exp_;
  memcpy(this, &b, sizeof(b));
  if (b.buf_) {
    buf_ = new char[cap_];
    memcpy(GetData(), b.Begin(), b.GetSize());
  }
}

Buffer& Buffer::Absorb(Buffer& b) {
 if (&b != this) {
   if (GetSize() == 0) {
    char buf[sizeof(b)];
    memcpy(buf, this, sizeof(b));
    memcpy(this, &b, sizeof(b));
    memcpy(&b, buf, sizeof(b));
    std::swap(exp_, b.exp_);
   } else {
    Append(b.Begin(), b.GetSize());
    b.Clear();
   }
 } 

 return *this;
}
}
