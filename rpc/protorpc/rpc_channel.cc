#include "rpc_channel.h"
#include "log.h"
#include "channel.h"
#include "rpc.pb.h"
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>

namespace rpc {

using namespace std::placeholders;

RpcChannel::RpcChannel() :
    codec_(std::bind(&RpcChannel::OnRpcMessage, this, _1, _2)),
    services_(nullptr)
{}

RpcChannel::RpcChannel(const TcpConnPtr& conn) :
    codec_(std::bind(&RpcChannel::OnRpcMessage, this, _1, _2)),
    conn_(conn),
    services_(nullptr)
{}

RpcChannel::~RpcChannel() {
    INFO("RpcChannel::dtor - %p", this);
    for (auto& iter : outstanding_) {
        auto out = iter.second;
        delete out.response;
        delete out.done;
    }
}

void RpcChannel::OnMessage(const TcpConnPtr& conn) {

}

void RpcChannel::CallMethod(const ::google::protobuf::MethodDescriptor* method,
                            ::google::protobuf::RpcController* controller,
                            const ::google::protobuf::Message* request,
                            ::google::protobuf::Message* response,
                            ::google::protobuf::Closure* done) {

}

void RpcChannel::OnRpcMessage(const TcpConnPtr& conn, const RpcMessagePtr& message) {

}


void RpcChannel::DoneCallback(::google::protobuf::Message* response, int64_t id) {

}

}
