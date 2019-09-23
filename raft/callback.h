#ifndef CALLBACK_H
#define CALLBACK_H

#include "raft.pb.h"
#include <functional>
namespace jraft {

typedef std::function<void(RequestVoteRequest&, RequestVoteResponse&)> OnPreVoteCallback;
typedef std::function<void(RequestVoteRequest&, RequestVoteResponse&)> OnRequestVoteCallback;
typedef std::function<void(AppendEntriesRequest&, AppendEntriesResponse&)> OnAppendEntriesCallback;

typedef std::function<void(const RequestVoteRequest&, RequestVoteResponse*)> PreVoteCallback;
typedef std::function<void(const RequestVoteRequest&, RequestVoteResponse*)> RequestVoteCallback;
typedef std::function<void(const AppendEntriesRequest&, AppendEntriesResponse*)> AppendEntriesCallback;
}
#endif
