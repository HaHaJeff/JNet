#ifndef NODE_H
#define NODE_H

#include "raft.h"
#include "rpc_server.h"
#include "raft_service.h"

namespace jraft {

// 
// Node as RpcServer
//
class Node {
public:
    Node(const Config& config, jnet::EventLoop* loop);
    ~Node();
    void Propose(const std::string& str);

    // 
    // regeister callback
    //
    void RequestVote(const RequestVoteRequest&, RequestVoteResponse*);
    void OnRequestVote(RequestVoteRequest&, RequestVoteResponse&);
    void PreVote(const RequestVoteRequest&, RequestVoteResponse*);
    void OnPreVote(RequestVoteRequest&, RequestVoteResponse&);
    void AppendEntries(const AppendEntriesRequest&, AppendEntriesResponse*);
    void OnAppendEntries(AppendEntriesRequest&, AppendEntriesResponse&);

    void StartServer();
    void Start();
private:
    Raft* raft_;
    int id_;
    jnet::EventLoop* loop_;
    jrpc::RpcServer server_;
    RaftServiceImpl service_;
    std::vector<RaftPeer*> peers_;
};

}

#endif