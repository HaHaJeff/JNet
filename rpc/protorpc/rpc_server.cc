#include "rpc_server.h"
#include "rpc_channel.h"
#include <google/protobuf/descriptor.h>
#include <google/protobuf/service.h>

namespace jrpc {

using namespace std::placeholders;

RpcServer::RpcServer(EventLoop* loop, const Ip4Addr& addr):
    server_(loop, addr)
{
    server_.OnConnCreate(std::bind(&RpcServer::OnConnection, this, _1));
}

void RpcServer::RegisterService(::google::protobuf::Service* service) {
    const ::google::protobuf::ServiceDescriptor* desc = service->GetDescriptor();
    services_[desc->full_name()] = service;
}

void RpcServer::Start(bool reusePort) {
    server_.Start(reusePort);
}

void RpcServer::OnConnection(const TcpConnPtr& conn) {

    RpcChannelPtr channel(new RpcChannel(conn));
    channel->SetServices(&services_);
    conn->OnRead(std::bind(&RpcChannel::OnMessage, channel.get(), _1));
    // 
    // ensure channel is always livness
    //
    conn->SetContext(channel);
}
}
