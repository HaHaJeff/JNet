#ifndef RPCCHANNEL_H
#define RPCCHANNEL_H
#include "tcpconn.h"
#include "rpc_codec.h"
#include <string>
#include <map>
#include <memory>

namespace google {
namespace protobuf {
    class Service;
}
}

class RpcChannel {
public:
    RpcChannel(const TcpConnPtr& conn) : conn_(conn), services_(nullptr) {}
    void SetServices(const std::map<std::string, ::google::protobuf::Service*>* services) {
        services_ = services;
    }
    void OnMessage(const TcpConnPtr& conn) {}
private:
    void OnRpcMessage(const TcpConnPtr& conn, const RpcMessagePtr& message) {}
private:
    TcpConnPtr conn_;
    const std::map<std::string, ::google::protobuf::Service*>* services_;
};

typedef std::shared_ptr<RpcChannel> RpcChannelPtr;

#endif
