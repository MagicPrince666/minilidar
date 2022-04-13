#pragma once

#include <sys/timerfd.h>
#include "xepoll.h"
#include "vl53l0x.h"
#include "mpu6050.h"
#include "common.h"

class TimerFd {
public:
    TimerFd();
    ~TimerFd();

    bool init();
    int timeOutCallBack();

private:
    int timer_fd_{-1};
    Vl53l0x *vl53l0x_;
    Mpu6050 *mpu6050_;
    LcdRgb *lcd_;
    std::string CurrentTime();
};

