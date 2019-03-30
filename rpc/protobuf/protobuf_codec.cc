#include "protobuf_codec.h"
#include <google/protobuf/message.h>

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

void ProtobufCodec::FillEmptyBuffer(const ::google::protobuf::Message& message, Buffer* buf) {
    assert(buf->GetSize() == 0);
    size_t byte_size = SerializeToBuffer(message, buf);
    // thanks muduo, the design of prependable bytes is so cute
    buf->Prepend(&byte_size, sizeof(size_t));
}

int ProtobufCodec::SerializeToBuffer(const ::google::protobuf::Message& message, Buffer* buf) {

    int byte_size = message.ByteSize();

    uint8_t* start = reinterpret_cast<uint8_t*>(buf->Begin());
    uint8_t *end = message.SerializeWithCachedSizesToArray(start); 
    buf->AddSize(byte_size);

    return byte_size;
}

int ProtobufCodec::ParseFromBuffer(::google::protobuf::Message& message, const Buffer* buf) {
    return 0;
}
