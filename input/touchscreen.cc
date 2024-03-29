#include <assert.h>
#include <dirent.h>
#include <fcntl.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <linux/input.h>
#include <linux/types.h>

#include "touchscreen.h"
#include "utils.h"

TouchScreen::TouchScreen()
{
}

TouchScreen::~TouchScreen(void)
{
    if (key_input_fd_ > 0) {
        MY_EPOLL.EpollDel(key_input_fd_);
        close(key_input_fd_);
    }
}

int TouchScreen::ReadTouchData(void)
{
    struct timeval time;
    struct input_event event;
    int ret = read(key_input_fd_, &event, sizeof(struct input_event));
    if (ret > 0) {
#if defined(__GLIBC__) && !defined(__UCLIBC__) && !defined(__MUSL__)
        time = event.time;
#else
        time.tv_usec = event.input_event_usec;
#endif
        switch (event.type) {
        case EV_SYN: // 分隔
            break;

        case EV_ABS: { // 可以读取坐标点了
            switch (event.code) {
            case ABS_X: // 读取x轴坐标
                std::cout << RED << "abs_x: " << event.value << "\n";
                break;

            case ABS_Y: // 读取y轴坐标
            {
                std::cout << GREEN << "abs_y: " << event.value << "\n";
            } break;

            case ABS_PRESSURE: // 按下/松开
            {
                if (event.value == 1) {
                    // 按下
                } else {
                    // 松开
                }
            } break;

            default:
                break;
            }
        } break;

        default:
            break;
        }
        // printf( "timeS=%d,timeUS=%d,type=%d,code=%d,value=%d\n", time.tv_sec, time.tv_usec, event.type, event.code, event.value );
    }
    return ret;
}

bool TouchScreen::Init()
{
    char buf[256] = {
        0,
    }; /* RATS: Use ok */
    int fd          = -1;
    std::string dev = "";
    std::vector<std::string> events;
    Utils::getFiles("/dev/input/", events);
    for (auto iter : events) {
        std::cout << "Device name " << iter << std::endl;
        if ((fd = open(iter.c_str(), O_RDONLY, 0)) >= 0) {
            ioctl(fd, EVIOCGNAME(sizeof(buf)), buf);
            dev = buf;
            std::cout << "Device info " << dev << std::endl;
            if (dev.find("TouchScreen") != std::string::npos || dev.find("Touchscreen") != std::string::npos) {
                key_input_fd_ = fd;
                break;
            }
            close(fd);
        }
    }
    assert(key_input_fd_ >= 0);
    // 绑定回调函数
    if (key_input_fd_ > 0) {
        std::cout << "Bind epoll" << std::endl;
        MY_EPOLL.EpollAdd(key_input_fd_, std::bind(&TouchScreen::ReadTouchData, this));
    }
    return true;
}
