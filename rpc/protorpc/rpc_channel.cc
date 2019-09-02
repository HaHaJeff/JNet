#include "rpc_channel.h"
#include "log.h"
#include "channel.h"
#include "rpc.pb.h"
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>

namespace jrpc {

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
    for (auto& iter : outstandings_) {
        auto out = iter.second;
        delete out.response;
        delete out.done;
    }
}

void RpcChannel::OnMessage(const TcpConnPtr& conn) {
    // consume conn input buffer
    codec_.OnMessage(conn);
}

void RpcChannel::CallMethod(const ::google::protobuf::MethodDescriptor* method,
                            ::google::protobuf::RpcController* controller,
                            const ::google::protobuf::Message* request,
                            ::google::protobuf::Message* response,
                            ::google::protobuf::Closure* done)
{
    RpcMessage message;
    message.set_type(REQUEST);
    message.set_id(id_++);
    message.set_service(method->service()->full_name());
    message.set_method(method->name());
    message.set_request(request->SerializeAsString());

    // async, when rpc is finished, done should be called in client
    OutstandingCall out = { response, done };
    {
        std::lock_guard<std::mutex> lck(mtx_);
        outstandings_[id_-1] = out;
    }
    codec_.Send(conn_, message);
}

void RpcChannel::OnRpcMessage(const TcpConnPtr& conn, const RpcMessagePtr& messagePtr) {
    // must handle same tcpconn
    assert(conn == conn_);

    //RpcMessage& message = *messagePtr;
    RpcMessage& message = *messagePtr;

    // for client
    if (message.type() == RESPONSE) {
        int64_t id = message.id();

        OutstandingCall out = { nullptr, nullptr };
        {
            std::lock_guard<std::mutex> lck(mtx_);
            auto iter = outstandings_.find(id);
            assert(iter != outstandings_.end());
            out = iter->second;
            outstandings_.erase(iter);
        }

        if (out.response != nullptr) {
            // To prevent memory leak, response is allocated in heap
            // and it valid until call done
            std::unique_ptr<::google::protobuf::Message> s(out.response);

            out.response->ParseFromString(message.response());
            out.done->Run();
        }
    } 
    // for server
    else if (message.type() == REQUEST) {
        assert(services_ != nullptr);
        auto iter = services_->find(message.service());
        assert(iter != services_->end());
        auto service = iter->second;

        // use reflect by protobuf
        auto desc    = service->GetDescriptor();
        auto method  = desc->FindMethodByName(message.method());

        if (method != nullptr) {
            std::unique_ptr<::google::protobuf::Message> request(service->GetRequestPrototype(method).New());
            if (request->ParseFromString(message.request())) {
                auto response = service->GetResponsePrototype(method).New();
                int64_t id = message.id();

                //DoneCallback is called after handle method
                //For example, EchoServiceImpl should override Echo, and call done->Run() in it
                service->CallMethod(method, nullptr, request.get(), response, NewCallback(this, &RpcChannel::DoneCallback, response, id));
            }
        }
    } else {
        //TODO: handle error
    }
}

void RpcChannel::DoneCallback(::google::protobuf::Message* response, int64_t id) {
    // prevent memory leak
    std::unique_ptr<::google::protobuf::Message> s(response);

    RpcMessage message;
    message.set_type(RESPONSE);
    message.set_id(id);
    message.set_response(response->SerializeAsString());
    codec_.Send(conn_, message);
}

}
