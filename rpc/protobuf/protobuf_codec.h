#ifndef PROTOBUFCODEC
#define PROTOBUFCODEC

#include "util.h"
#include "timestamp.h"
#include "slice.h"
#include "buffer.h"
#include "channel.h"
#include "net/log.h"
#include "tcpconn.h"
#include "rpc.pb.h"
#include <memory>

namespace google
{
namespace protobuf {
class Message;
}
}


using namespace jnet;
namespace jrpc {

const int32_t kHeaderLen = 4;
typedef std::shared_ptr<::google::protobuf::Message> MessagePtr;
using namespace std::placeholders;

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
    int ParseFromBuffer(::google::protobuf::Message* message, const char* buf, size_t len);

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

// 
// Thanks muduo
// ProtobufCodecT模板类
//

template<typename MSG, typename CODEC=ProtobufCodec>
class ProtobufCodecT {
public:
    typedef std::shared_ptr<MSG> ConcreteMessagePtr;
    typedef std::function<void (const TcpConnPtr&, const ConcreteMessagePtr&)> ProtobufMessageCallback;
    
    explicit ProtobufCodecT(const ProtobufMessageCallback& msgCb)
        : messageCallback_(msgCb),
          codec_(&MSG::default_instance(), std::bind(&ProtobufCodecT::OnRpcMessage, this, _1, _2)) 
    {
    }

    void Send(const TcpConnPtr& conn, const MSG& message) {
        codec_.Send(conn, message);
    }

    void OnMessage(const TcpConnPtr& conn) {
        codec_.OnMessage(conn);
    }

    void OnRpcMessage(const TcpConnPtr& conn, const MessagePtr& message) 
    {
        // 类型转换 down cast
        messageCallback_(conn, std::dynamic_pointer_cast<MSG>(message));
    }

private:
    ProtobufMessageCallback messageCallback_;
    CODEC codec_;
};

}

#endif
