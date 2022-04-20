#pragma once

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>
class Kqueue {
 public:
  static Kqueue* Instance();
  
  ~Kqueue();

  void setNonBlock(int fd);
  void updateEvents(int fd, int events, bool modify);
  void handleAccept(int fd);
  void handleRead(int fd);
  void handleWrite(int fd);
  void loop_once(int lfd, int waitms);

 private:
  Kqueue();
  int epollfd = -1;
  const int kReadEvent = 1;
  const int kWriteEvent = 2;
  static Kqueue *instance_;
  std::unordered_map<int, std::function<void()>> listeners_;
};