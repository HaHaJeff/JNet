#ifndef CALLBACK_H
#define CALLBACK_H

#include "raft.pb.h"
#include <functional>
namespace jraft {

typedef std::function<void(int, const RequestVoteRequest&, const RequestVoteReply&)> RequestVoteCallback;
typedef std::function<void(int, const AppendEntriesRequest&, const AppendEntriesReply&)> AppendEntriesCallback;
}
#endif
