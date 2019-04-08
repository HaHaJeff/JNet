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

// Abstract interface for an RPC channel.  An RpcChannel represents a
// communication line to a Service which can be used to call that Service's
// methods.  The Service may be running on another machine.  Normally, you
// should not call an RpcChannel directly, but instead construct a stub Service
// wrapping it.  Example:
//   RpcChannel* channel = new MyRpcChannel("remotehost.example.com:1234");
//   MyService* service = new MyService::Stub(channel);
//   service->MyMethod(request, &response, callback);
class RpcChannel : public ::google::protobuf::RpcChannel {
public:
    RpcChannel();
    RpcChannel(const TcpConnPtr& conn); 
    ~RpcChannel();    

    void SetConn(const TcpConnPtr& conn) { conn_ = conn; }

    //
    // Server channel need dispatch message
    //
    void SetServices(const std::map<std::string, ::google::protobuf::Service*>* services) {
        services_ = services;
    }

    //
    // Callback for tcpconn HandleRead
    //
    void OnMessage(const TcpConnPtr& conn);

    // Thanks google
    // Call the given method of the remote service.  The signature of this
    // procedure looks the same as Service::CallMethod(), but the requirements
    // are less strict in one important way:  the request and response objects
    // need not be of any specific class as long as their descriptors are
    // method->input_type() and method->output_type().
    void CallMethod(const ::google::protobuf::MethodDescriptor* method,
                    ::google::protobuf::RpcController* controller,
                    const ::google::protobuf::Message* request,
                    ::google::protobuf::Message* response,
                    ::google::protobuf::Closure* done) override;

private:
    // 
    // When get full rpc packet, call this function to specific service
    // Client need handle RpcMessage::type RESPONSE
    // Server need handle RpcMessage::type REQUEST
    //
    void OnRpcMessage(const TcpConnPtr& conn, const RpcMessagePtr& message);

    // 
    // Client need call this function when rpc is done, id unique specific a rpc call
    // Server need call this function to send response
    //
    void DoneCallback(::google::protobuf::Message* response, int64_t id);

private:
    //
    // Save rpc call info, response need store on heap
    //
    struct OutstandingCall {
        ::google::protobuf::Message* response;
        // google protobuf warp for function
        ::google::protobuf::Closure* done;
    };

    // 
    // For onMessage
    //
    RpcCodec codec_;
    TcpConnPtr conn_;
    std::atomic<int64_t> id_;
    
    std::mutex mtx_;

    std::map<int, OutstandingCall> outstandings_;
    // key: Service::full_name()
    const std::map<std::string, ::google::protobuf::Service*>* services_;
    
};

typedef std::shared_ptr<RpcChannel> RpcChannelPtr;

}
#endif
