#include "protobuf_codec.h"
#include "../../net/buffer.h"
#include "../../net/tcpconn.h"
#include <google/protobuf/message.h>

ProtobufCodec::ProtobufCodec(const ::google::protobuf::Message* prototype,
                             ProtobufMessageCallback& messagecallback)
    : prototype_(prototype),
      messageCallback_(messageCallback)
{
}
    
void ProtobufCodec::Send(const TcpConnPtr& conn, const ::google::protobuf::Message& message) {
    Buffer buf;
    FillEmptyBuffer(&buf, message);
    conn->Send(buf);
}

void ProtobufCodec::FillEmptyBuffer(Buffer* buf, const ::google::protobuf::Message& message) {
    assert(buf->GetSize() == 0);
    int byte_size = SerializeToBuffer(message, buf);
}

int ProtobufCodec::SerializeToBuffer(const ::google::protobuf::Message& message, Buffer* buf) {
    return 0;
}

int ProtobufCodec::ParseFromBuffer(::google::protobuf::Message& message, const Buffer* buf) {
    return 0;
}
