#pragma once

#include <sys/timerfd.h>
#include "xepoll.h"

class TimerFd {
public:
    TimerFd();
    ~TimerFd();

    bool init();
    int timeOutCallBack();

private:
    int timer_fd_{-1};
    std::string CurrentTime();
};

