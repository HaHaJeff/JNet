#ifndef CALLBACK_H
#define CALLBACK_H

#include "raft.pb.h"
#include <functional>
namespace jraft {

typedef std::function<void(const RequestVoteRequest&, const RequestVoteResponse&)> OnPreVoteCallback;
typedef std::function<void(const RequestVoteRequest&, const RequestVoteResponse&)> OnRequestVoteCallback;
typedef std::function<void(const AppendEntriesRequest&, const AppendEntriesResponse&)> OnAppendEntriesCallback;

typedef std::function<void(const RequestVoteRequest&, RequestVoteResponse&)> PreVoteCallback;
typedef std::function<void(const RequestVoteRequest&, RequestVoteResponse&)> RequestVoteCallback;
typedef std::function<void(const AppendEntriesRequest&, AppendEntriesResponse&)> AppendEntriesCallback;
}
#endif
