#ifndef CONFIG_H
#define CONFIG_H

// 
// for Ip4Addr
//
#include "net.h"
#include <vector>

using namespace jnet;

namespace jraft {

struct Config {
   
    //
    // unique id for raft node
    //
    int id_;

    // 
    // persistence data storage path for each raft node
    //
    std::string path_;

    //
    // leader heartbeat timeout (tick)
    // heartbeat timeout must < election tiomeout
    //
    int64_t heartbeatTimeout_ = 1;

    //
    // election timeout of each raft node
    //
    int64_t electionTimeout_ = 5;

    //
    // unit of each tick
    //
    int64_t millisecond_ = 100;

    //
    // RPC server adress of this raft node
    //
    Ip4Addr serverAddress_;

    //
    // peer address exclude this raft node
    //
    std::vector<Ip4Addr> peerAddresses_;
};

}

#endif
