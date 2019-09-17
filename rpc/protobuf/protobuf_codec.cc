#include "protobuf_codec.h"
#include <google/protobuf/message.h>
#include <arpa/inet.h>

namespace jrpc {

ProtobufCodec::ProtobufCodec(const ::google::protobuf::Message* prototype,
                             const ProtobufMessageCallback& messageCallback)
    : prototype_(prototype),
      messageCallback_(messageCallback)
{
}
    
void ProtobufCodec::Send(const TcpConnPtr& conn, const ::google::protobuf::Message& message) {
    Buffer buf;
    FillEmptyBuffer(message, &buf);
    conn->Send(buf);
}

void ProtobufCodec::OnMessage(const TcpConnPtr& conn) {
    Buffer& buf = conn->GetInput();
    const int32_t len = buf.PeekInt32();
    //
    // receive a full packet
    //
    if (buf.GetSize() >= static_cast<size_t>(len)) {
        // TODO: fix this bug, MessagePtr dtor cause core dump, the reason is that call messageCallback_ will call ProtobufCodecT::OnRpcMessage,
        // this function invloved with dynamic_cast, my solution is use raw pointer with dynamic_cast then wrapper shared_ptr,
        // so the raw pointer will be deleted after this function, finally, after messageCallback_, message dtor will core dump
        MessagePtr message(prototype_->New());
        ParseFromBuffer(message.get(), buf.GetData()+kHeaderLen, len);
        messageCallback_(conn, message);
        buf.Consume(len+kHeaderLen);
    }
}

void ProtobufCodec::FillEmptyBuffer(const ::google::protobuf::Message& message, Buffer* buf) {
    assert(buf->GetSize() == 0);
    buf->AppendInt32(0);
    size_t byte_size = SerializeToBuffer(message, buf);
    // thanks muduo, the design of prependable bytes is so cute
    int32_t* ptr = reinterpret_cast<int32_t*>(buf->Begin());
    int32_t be = htonl(byte_size);
    ::memcpy(ptr, &be, sizeof(int32_t));
}

int ProtobufCodec::SerializeToBuffer(const ::google::protobuf::Message& message, Buffer* buf) {

    int byte_size = message.ByteSize();
    uint8_t* start = reinterpret_cast<uint8_t*>(buf->Begin()+kHeaderLen);
    uint8_t *end = message.SerializeWithCachedSizesToArray(start); 
    buf->AddSize(byte_size);
    
    return byte_size;
}

int ProtobufCodec::ParseFromBuffer(::google::protobuf::Message* message, const char* buf, size_t len) {
    return message->ParseFromArray(buf, len);
}

}
