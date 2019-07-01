#include <iostream>
#include <poll.h>

#include "socket.h"
#include "net.h"
#include "buffer.h"
#include "eventloop.h"
#include "net/log.h"
#include "timer.h"
#include "timestamp.h"
#include "channel.h"
#include "processinfo.h"
#include "unit_test.h"
#include "timerqueue.h"
#include "eventloop_thread.h"
#include "eventloop_pool.h"

#include "storage.h"
#include "raft/log.h"
#include "raft/state.pb.h"

using namespace jnet;
using namespace jraft;
/*
TEST(TestBase, TimeStamp) {
  TimeStamp t(TimeStamp::Now());
  std::cout << t.ToString() << std::endl;
  std::cout << t.ToFormattedString() << std::endl;
}

TEST(TestBase, Channel) {
  Channel ch(nullptr, -1);
  ch.SetREvents(POLLNVAL);
  ch.HandleEvent();
}

TEST(TestBase, Buffer) {
  Buffer buf;
  char b[1024] = "hellow, world";
  buf.Append(b, 1024);
  buf.Consume(7);
  std::cout << buf.GetData() << std::endl;
}

TEST(TestBase, Ip4Addr) {
  Ip4Addr ip4("127.0.0.1", 9999);
  std::cout << ip4.ToString() << std::endl;
}

TEST(TestBase, ProcessInfo) {
  std::cout << ProcessInfo::hostName() << std::endl;
}

TEST(TestBase, Socket) {
  int fd = socket(AF_INET, SOCK_STREAM, 0);
  Socket s(fd);
  s.SetKeepAlive(true);
  s.SetReuseAddr(true);
  s.SetReusePort(true);
  Ip4Addr ip4("127.0.0.1", 9999);
  s.BindAddress(ip4);
  s.Listen();
  Ip4Addr peer;
  int connfd = s.Accept(peer);
  char buf[1024];
  std::cout << "Socket tcp info: ";
  s.GetTcpInfoString(buf, 1024);
  std::cout << buf << std::endl;

  bzero(buf, sizeof(buf));

  int ret = 0;
  while (1) {
    ret = read(connfd, buf, sizeof(buf));
    if (ret == -1) {
      std::cout << "ret == -1" << std::endl;
      continue;
    } else {
      break;
    }
  }
  std::cout << "read bytes: " << ret << " content: "<<  buf << std::endl;
  ret = send(connfd, buf, 4, 0);
  if (ret == -1) perror("send ");
}

TEST(TestBase, TimerQueue) {
    EventLoop* loop = new EventLoop();
    TimerQueue queue(loop);
    Timer *timer = new Timer([=]{std::cout << "Timer" << std::endl;}, TimeStamp(TimeStamp::Now()), 0.0);
    queue.AddTimerInLoop(timer);
    queue.HandleRead();
}

TEST(TestBase, EventLoop) {
  Logger& log = Logger::GetLogger();
  log.SetFileName("event.log");
  EventLoop loop;

  int i = 0;
  while (i < 1) {
    loop.RunEvery(0.0, 1.0, [i]
        {
          std::cout << "RunEvery: " << i << std::endl;
        }
        );
    i+=1;
  }

  loop.Loop();
}

TEST(TestBase, EventLoopPool) {
  Logger& log = Logger::GetLogger();
  log.SetFileName("event.log");
  EventLoop baseLoop;
  EventLoopPool pool(&baseLoop);
  pool.SetThreadNum(2);
  pool.Start();
  auto loop = pool.GetNextLoop();
  int i = 0;
  while (i < 1) {
    loop->RunEvery(0.0, 1.0, [i]
        {
          std::cout << "RunAfter: " << i << std::endl;
        }
        );
    i+=1;
  }
  baseLoop.Loop();
}
TEST(TestBase, EventLoopThread) {
  Logger& log = Logger::GetLogger();
  log.SetFileName("event.log");
  EventLoopThread t;
  auto loop = t.StartLoop();
  int i = 0;
  while (i < 1) {
    loop->RunAfter(2.0, [i]
        {
          std::cout << "RunAfter: " << i << std::endl;
        }
        );
    i+=1;
  }

    while(1); 
}
*/

TEST(TestBase, Storage) {

    LogEntry entry;
    entry.set_type(EntryType::ENTRY_TYPE_DATA);
    entry.set_term(2);
    entry.set_index(3);
    entry.set_command("gaobosong");

    Storage storage("test_storage");
    storage.PutCurrentTerm(2);
    storage.PutVotedFor(0);
    storage.PutFirstIndex(3);
    storage.PutLastIndex(3);
    storage.PutEntry(entry);

    std::vector<LogEntry> ans = storage.GetEntry();

    for (auto& v : ans) {
      std::cout << "term:" << v.term() << std::endl;
      std::cout << "index:" << v.index() << std::endl;
      std::cout << "command:" << v.command() << std::endl;
    }
}

int main()
{
  RunAllTests();
}
