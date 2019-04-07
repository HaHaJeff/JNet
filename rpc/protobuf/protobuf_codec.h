#ifndef PROTOBUFCODEC
#define PROTOBUFCODEC

#include "util.h"
#include "timestamp.h"
#include "slice.h"
#include "buffer.h"
#include "channel.h"
#include "log.h"
#include "tcpconn.h"
#include <memory>

namespace google
{
namespace protobuf {
class Message;
}
}


namespace rpc {

typedef std::shared_ptr<::google::protobuf::Message> MessagePtr;

class ProtobufCodec {
public:
    // 
    // call ProtobufMessageCallback after Codec get the full packet
    // TODO: add Message arguement
    //
    typedef std::function<void (const TcpConnPtr&, const MessagePtr&)> ProtobufMessageCallback;
    ProtobufCodec(const ::google::protobuf::Message* prototype, const ProtobufMessageCallback& msgcallback);

    // 
    // Send serialize message into Buffer, then call TcpConn send
    //
    void Send(const TcpConnPtr& conn, const ::google::protobuf::Message& message);

    //
    // if buf->size() >= packet len, call messageCallback_
    //
    void OnMessage(const TcpConnPtr& conn); 
private:

    //
    // len + payload, simpliefy the transform data format
    //
    void FillEmptyBuffer(const ::google::protobuf::Message& message, Buffer* buf);

    // 
    // just only serialize ::google::protobuf::Message
    //
    int SerializeToBuffer(const ::google::protobuf::Message& message, Buffer* buf);

    // 
    // just parse buf.data + kHeadLen(in prependable bytes) into message
    //
    int ParseFromBuffer(::google::protobuf::Message& message, const char* buf, size_t len);

private:
    //
    // TODO
    //
    static int32_t Checksum(const void* buf, int len) {return 0;}

    // 
    // TODO
    // 
    static bool ValidateChecksum(const char* buf, int len) {return false;} 

    static const int kHeaderLen = 4;

private:
    const ::google::protobuf::Message* prototype_;
    ProtobufMessageCallback messageCallback_;
};
}

#endif
