#pragma once

#include <sys/timerfd.h>
#include "xepoll.h"

class TimerFd {
public:
    TimerFd(Xepoll *epoll);
    ~TimerFd();

    bool init();
    int timeOutCallBack();

private:
    Xepoll *epoll_;
    int timer_fd_{-1};
};

