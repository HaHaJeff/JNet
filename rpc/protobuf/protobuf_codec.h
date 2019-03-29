#ifndef PROTOBUFCODEC
#define PROTOBUFCODEC

#include "../../net/util.h"
#include "../../net/timestamp.h"
#include "../../net/slice.h"
#include <memory>

namespace google
{
namespace protobuf {
class Message;
}
}

typedef std::shared_ptr<::google::protobuf::Message> MessagePtr;

class ProtobufCodec {
public:
    // ProtobufMessageCallback在Codec成功解析消息后调用
    typedef std::function<void (const TcpConnPtr&, TimeStamp)> ProtobufMessageCallback;
    ProtobufCodec(const ::google::protobuf::Message* prototype, Slice tag, ProtobufMessageCallback& msgcallback);

    void Send(const TcpConnPtr& conn, const ::google::protobuf::Message& message);
    void onMessage(const TcpConnPtr& conn, Buffer* buf, TimeStamp receiveTime); 

private:
    void FillEmptyBuffer(Buffer* buf, const ::google::protobuf::Message& message);
    int SerializeToBuffer(const ::google::protobuf::Message& message, Buffer* buf);
    int ParseFromBuffer(const Buffer* buf, ::google::protobuf::Message& message);

private:
    static int32_t Checksum(const void* buf, int len) {return 0;}
    static bool ValidateChecksum(const char* buf, int len) {return false;} 

private:
    const ::google::protobuf::Message* prototype_;
    ProtobufMessageCallback messageCallback_;
};

#endif
