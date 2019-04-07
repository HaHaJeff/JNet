#ifndef RPCCODEC_H
#define RPCCODEC_H

class RpcMessage;
typedef std::shared_ptr<RpcMessage> RpcMessagePtr;

#include "protobuf_codec.h"
namespace rpc {

typedef ProtobufCodecT<RpcMessage> RpcCodec; 

}

#endif
