#include <iostream>
#include <linux/input.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <iomanip>
#include <assert.h>

#include "ultrasonic.h"
#include "utils.h"

Ultrasonic::Ultrasonic()
{
    char          buf[256] = { 0, };  /* RATS: Use ok */
    int           fd = -1;
    std::string dev = "";
    std::vector<std::string> events;
    Utils::getFiles("/dev/input/", events);
    for (auto iter : events) {
        std::cout << "Device name "<< iter << std::endl;
        if ((fd = open(iter.c_str(), O_RDONLY, 0)) >= 0) {
            ioctl(fd, EVIOCGNAME(sizeof(buf)), buf);
            dev = buf;
            std::cout << "Device info "<< dev << std::endl;
            if(dev == "gpio-keys") {
                key_input_fd_ = fd;
                break;
            }
            close(fd);
        }
    }

    assert(fd >= 0);

    init();
}

Ultrasonic::~Ultrasonic(void)
{
    if (key_input_fd_ > 0) {
        MY_EPOLL->EpollDel(key_input_fd_);
        close(key_input_fd_);
    }
}

int Ultrasonic::IRKey(void)
{
    struct input_event key;
    int ret = read(key_input_fd_, &key, sizeof(key));
    if(ret > 0) {
        if(key.code == 103) {
            // std::cout << "Value = " << key.value <<std::endl;
            // std::cout << "Time = " << key.time.tv_sec << "." << (key.time.tv_usec)/1000 << " s" <<std::endl;

            if(key.value == 1) { // 记录开始时间
                last_time_.tv_sec = key.input_event_sec;
                last_time_.tv_usec = key.input_event_usec;
            } else {
                // 换算距离
                double time = 0.0;
                double starttime = last_time_.tv_sec * 1000000 + last_time_.tv_usec;
                double endtime = key.input_event_sec * 1000000 + key.input_event_usec;

                time = (endtime - starttime) * 0.0001;

                double distance = 170.0 * time * 0.0001;
                std::cout << "Time = " << time << " ms" <<std::endl;
                std::cout << "Distance = " << std::setprecision(3) << distance << " m" <<std::endl;
            }
        }
    }
    return ret;
}

bool Ultrasonic::init() {
  // 绑定回调函数
  if (key_input_fd_ > 0) {
        std::cout << "Bind epoll" << std::endl;
        MY_EPOLL->EpollAdd(key_input_fd_, std::bind(&Ultrasonic::IRKey, this));
  }
  return true;
}

void Ultrasonic::Transfer(int num) {
    is_action_ = num;
}
