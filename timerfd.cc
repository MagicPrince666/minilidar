#include <iostream>
#include <fstream>
#include <unistd.h>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <stdexcept>
#include <vector>
#include <chrono>
#include <codecvt>
#include <string>
#include <iomanip>
#include <algorithm>
#include <sstream>
#include <sys/statfs.h>
#include <assert.h>

#include "timerfd.h"

TimerFd::TimerFd()
{
    if(!init()) {
      std::cout << "timerfd init failed!" << std::endl;
      assert(timer_fd_ >= 0);
    }
    // vl53l0x_ = new (std::nothrow)Vl53l0x;
    // mpu6050_ = new (std::nothrow)Mpu6050;
    lcd_ = new (std::nothrow) LcdRgb(0);
    std::string title = "Huang liquan ";
	title += CurrentTime();
    lcd_->FillScreenSolid(0x0000ff);
	lcd_->FbPutString(30, 0, title.c_str(), title.size(), RGB_GOLDEN, true, title.size());
}

TimerFd::~TimerFd()
{
    if (timer_fd_) {
        MY_EPOLL->EpollDel(timer_fd_);
        close(timer_fd_);
    }
    if(vl53l0x_) {
        delete vl53l0x_;
    }
    if(mpu6050_) {
        delete mpu6050_;
    }
    if(lcd_) {
        delete lcd_;
    }
}

std::string TimerFd::CurrentTime()
{
	std::stringstream ss;
	std::string str = "%Y-%m-%d %H-%M-%S";
	std::chrono::system_clock::time_point a = std::chrono::system_clock::now();      //时间点可以做差
	time_t t1 = std::chrono::system_clock::to_time_t(a);				  //time_t可以格式化
	ss << std::put_time(localtime(&t1), str.c_str());
	std::string str1 = ss.str();
	return str1;
}

bool TimerFd::init() {
    // 创建1s定时器fd
    if ((timer_fd_ =
            timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC)) < 0) {
        std::cout << "create timer fd fail" << std::endl;
        return false;
    }
    // 设置1s定时器
    struct itimerspec time_intv;
    time_intv.it_value.tv_sec = 0;  //设定超时
    time_intv.it_value.tv_nsec = 100000000; // 100ms
    time_intv.it_interval.tv_sec = time_intv.it_value.tv_sec;  //间隔超时
    time_intv.it_interval.tv_nsec = time_intv.it_value.tv_nsec;
    // 启动定时器
    timerfd_settime(timer_fd_, 0, &time_intv, NULL);
    // 绑定回调函数
    MY_EPOLL->EpollAdd(timer_fd_, std::bind(&TimerFd::timeOutCallBack, this));

    return true;
}

int TimerFd::timeOutCallBack() {
    // 需要读出该fd的数据，否则定时器无法正常执行(重要)
    uint64_t value;
    int ret = read(timer_fd_, &value, sizeof(uint64_t));

    int distance = vl53l0x_->GetDistance();
    lcd_->FbPutValue(30, 20, distance, 5, RGB_VERMILION, true, 5);

    int gx,gy,gz;
    int ax,ay,az;
    mpu6050_->MpuGetGyroscope(gx, gy, gz);
    mpu6050_->MpuGetAccelerometer(ax, ay, az);
    lcd_->FbPutValue(30, 40, gx, 5, RGB_GOLDEN, true, 5);
    lcd_->FbPutValue(30, 60, gy, 5, RGB_GOLDEN, true, 5);
    lcd_->FbPutValue(30, 80, gz, 5, RGB_GOLDEN, true, 5);

    lcd_->FbPutValue(100, 40, ax, 5, RGB_GOLDEN, true, 5);
    lcd_->FbPutValue(100, 60, ay, 5, RGB_GOLDEN, true, 5);
    lcd_->FbPutValue(100, 80, az, 5, RGB_GOLDEN, true, 5);

    return ret;
}