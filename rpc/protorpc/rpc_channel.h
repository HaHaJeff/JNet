#ifndef RPCCHANNEL_H
#define RPCCHANNEL_H

#include "tcpconn.h"
#include "rpc_codec.h"

#include <string>
#include <map>
#include <memory>
#include <atomic>

#include <google/protobuf/service.h>

namespace google {
namespace protobuf {
    class Service;
    class Message;
    class RpcController;
    class Descriptor;
    class ServiceDescriptor;
    class MethodDescriptor;
    class Closure;
    class RpcChannel;
}
}

namespace rpc {

class RpcChannel : public ::google::protobuf::RpcChannel {
public:
    RpcChannel() {}
    RpcChannel(const TcpConnPtr& conn) : conn_(conn), services_(nullptr) {}
    
    void SetConn(const TcpConnPtr& conn) { conn_ = conn; }

    void SetServices(const std::map<std::string, ::google::protobuf::Service*>* services) {
        services_ = services;
    }

    void OnMessage(const TcpConnPtr& conn) {}

    void CallMethod(const ::google::protobuf::MethodDescriptor* method,
                    ::google::protobuf::RpcController* controller,
                    const ::google::protobuf::Message* request,
                    ::google::protobuf::Message* response,
                    ::google::protobuf::Closure* done) override{}

private:
    void OnRpcMessage(const TcpConnPtr& conn, const RpcMessagePtr& message) {}
    void DoneCallback(::google::protobuf::Message* response, int64_t id);

private:
    struct OutstandingCall {
        ::google::protobuf::Message* response;
        ::google::protobuf::Closure* done;
    };

    RpcCodec codec_;
    TcpConnPtr conn_;
    std::atomic<int> id_;

    std::map<int, OutstandingCall> outstanding_;
    const std::map<std::string, ::google::protobuf::Service*>* services_;
    
};

typedef std::shared_ptr<RpcChannel> RpcChannelPtr;

}
#endif
