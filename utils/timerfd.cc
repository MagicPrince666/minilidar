#include <algorithm>
#include <assert.h>
#include <chrono>
#include <codecvt>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <sys/statfs.h>
#include <unistd.h>
#include <vector>

#include "timerfd.h"

TimerFd::TimerFd()
{
    timer_fd_ = -1;
    std::cout << "timerfd init!" << std::endl;
}

TimerFd::~TimerFd()
{
    if (timer_fd_) {
        MY_EPOLL.EpollDel(timer_fd_);
        close(timer_fd_);
    }
}

std::string TimerFd::CurrentTime()
{
    std::stringstream ss;
    std::string str                         = "%Y-%m-%d %H-%M-%S";
    std::chrono::system_clock::time_point a = std::chrono::system_clock::now();        // 时间点可以做差
    time_t t1                               = std::chrono::system_clock::to_time_t(a); // time_t可以格式化
    ss << std::put_time(localtime(&t1), str.c_str());
    std::string str1 = ss.str();
    return str1;
}

bool TimerFd::init()
{
    // 创建1s定时器fd
    if ((timer_fd_ =
             timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC)) < 0) {
        std::cout << "create timer fd fail " << timer_fd_ << std::endl;
        return false;
    }
    assert(timer_fd_ >= 0);
    // 设置1s定时器
    struct itimerspec time_intv;
    time_intv.it_value.tv_sec     = 1;                         // 设定超时
    time_intv.it_value.tv_nsec    = 0;                         // 1s
    time_intv.it_interval.tv_sec  = time_intv.it_value.tv_sec; // 间隔超时
    time_intv.it_interval.tv_nsec = time_intv.it_value.tv_nsec;
    // 启动定时器
    timerfd_settime(timer_fd_, 0, &time_intv, NULL);
    // 绑定回调函数
    MY_EPOLL.EpollAdd(timer_fd_, std::bind(&TimerFd::timeOutCallBack, this));

    // mpu6050_  = std::make_shared<Mpu6050>();
    // mpu6050_->Init();

    // vl53l0x_ = std::make_shared<Vl53l0x>();

    // lcd_              = std::make_shared<LcdRgb>("/dev/fb0");
    // std::string title = "Huang liquan ";
    // title += CurrentTime();
    // lcd_->FillScreenSolid(0x0000ff);
    // lcd_->FbPutString(30, 0, title.c_str(), title.size(), RGB_GOLDEN, true, title.size());

    return true;
}

int TimerFd::timeOutCallBack()
{
    // 需要读出该fd的数据，否则定时器无法正常执行(重要)
    uint64_t value;
    int ret = read(timer_fd_, &value, sizeof(uint64_t));

    if (vl53l0x_) {
        int distance = vl53l0x_->GetDistance();
        lcd_->FbPutValue(30, 20, distance, 5, RGB_VERMILION, true, 5);
    }

    if (mpu6050_) {
        euler_t euler;
        mpu6050_->GetEulerQuat(euler);
        lcd_->FbPutValue(30, 40, euler.pitch, 5, RGB_GOLDEN, true, 5);
        lcd_->FbPutValue(30, 60, euler.roll, 5, RGB_GOLDEN, true, 5);
        lcd_->FbPutValue(30, 80, euler.yaw, 5, RGB_GOLDEN, true, 5);
    }

    return ret;
}