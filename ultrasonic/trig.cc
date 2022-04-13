#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>
#include <string.h>
#include <sys/types.h>
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <unistd.h>
#include <iostream>

#include "trig.h"

#define GPIO_PIN "/sys/class/leds/trig/brightness"

Trig::Trig(Interface *interface) : m_interface_(interface){
    if(access(GPIO_PIN, F_OK) == 0) {
        trig_fd_ = open (GPIO_PIN, O_RDWR);
        std::cout << "Init trig pin fd " << trig_fd_ << std::endl;
    }

    assert(trig_fd_ >= 0);
    
    write(trig_fd_, "0", 1);

    trig_thread_ = std::thread([](Trig* pThis){
        pThis->TrigProcess();
    }, this);
}

Trig::~Trig() {
    trig_thread_.join();
    if(trig_fd_ > 0) {
        close(trig_fd_);
    }
}

int Trig::Action() {
    if(trig_fd_ > 0) {
        write(trig_fd_, "1", 1);
        usleep(100);
        write(trig_fd_, "0", 1);
        return 0;
    } else {
        std::cout << "trig pin error trig_fd_ = " << trig_fd_ << std::endl;
        return -1;
    }
    return 0;
}

void  Trig::TrigProcess() {
    while(true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        Action();
        if(nullptr != m_interface_){
            m_interface_->Transfer(1);
        }
    }
}