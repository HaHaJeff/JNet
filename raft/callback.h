#ifndef CALLBACK_H
#define CALLBACK_H

#include "raft.pb.h"
#include <functional>
namespace jraft {

typedef std::function<void(int, const RequestVoteRequest&, const RequestVoteResponse&)> OnPrevoteCallback;
typedef std::function<void(int, const RequestVoteRequest&, const RequestVoteResponse&)> OnRequestVoteCallback;
typedef std::function<void(int, const AppendEntriesRequest&, const AppendEntriesResponse&)> OnAppendEntriesCallback;
}
#endif
