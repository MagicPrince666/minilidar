/**
 * @file inotify.h
 * @author 黄李全
 * @brief 
 * @version 0.1
 * @date 2021-06-10
 * @copyright 
 */
#pragma once

#include "xepoll.h"

class Inotify {
 public:
  Inotify(const std::string name);
  ~Inotify();

  int handle_event();

 private:
    int fd;
    int wd;
    int len;
    int nread;
    char buf[BUFSIZ];
    struct inotify_event *event;
};
