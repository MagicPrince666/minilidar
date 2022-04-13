#pragma once

#include <sys/socket.h>
#include <sys/epoll.h>

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

#define UDP_BUF_SIZE 1472
#define TCPBUF_SIZE 1460
#define MAXEVENTS 20
#define EPOLL_FD_SETSIZE    1024

#define MY_EPOLL Epoll::Instance()

class Epoll {
 public:
  static Epoll* Instance();
  
  ~Epoll();

  int EpollAdd(int fd, std::function<void()> handler);
  int EpollDel(int fd);
  int EpollLoop();
  bool EpoolQuit();

 private:
  Epoll();
  struct epoll_event ev_, events_[MAXEVENTS];
  int epfd_;
  int nfds_;
  bool epoll_loop_{true};
  static Epoll *instance_;
  std::unordered_map<int, std::function<void()>> listeners_;
};

