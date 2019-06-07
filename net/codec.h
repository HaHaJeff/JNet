#ifndef CODEC_H
#define CODEC_H

#include <string> 
#include "buffer.h"

namespace jnet {

class CodecBase {
public:
    virtual int TryDecode(const std::string& data,  std::string& msg) = 0;
    virtual void Encode(std::string msg, Buffer& buffer) = 0;
    virtual CodecBase* Clone() = 0;
};

class LineCode: public CodecBase {
public:
    int TryDecode(const std::string& data, std::string& msg) override;
    void Encode(std::string msg, Buffer& buf) override;
    CodecBase* Clone() override { return new LineCode(); }
};

class LengthCode: public CodecBase {
public:
    int TryDecode(const std::string& data, std::string& msg) override;
    void Encode(std::string msg, Buffer& buf) override;
    CodecBase* Clone() override { return new LengthCode(); }
};

}
#endif
