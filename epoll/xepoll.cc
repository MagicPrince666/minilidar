#include <fcntl.h>
#include <unistd.h>
#include <assert.h>

#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <string>

#include "xepoll.h"
Epoll *Epoll::instance_ = nullptr;

Epoll::Epoll(void) {
#ifndef __APPLE__
    epfd_ = ::epoll_create(EPOLL_FD_SETSIZE);
#else
    epfd_ = kqueue();
#endif
    assert(epfd_ >= 0);
}

Epoll::~Epoll(void)
{
    if (epfd_ > 0) {
        ::close(epfd_);
    }
    if (!instance_) {
        delete instance_;
    }
}

Epoll *Epoll::Instance()
{
    if (!instance_) {
        instance_ = new Epoll();
    }
    return instance_;
}

// 添加到epoll事件，默认设置为非阻塞且fd的端口和地址都设为复用
int Epoll::EpollAdd(int fd, std::function<void()> handler)
{
    listeners_[fd] = handler;

    // 设置为非阻塞
    int sta = ::fcntl(fd, F_GETFD, 0) | O_NONBLOCK;
    if (::fcntl(fd, F_SETFL, sta) < 0) {
        return -1;
    }
#ifndef __APPLE__
    ev_.data.fd    = fd;
    ev_.events     = EPOLLIN;
    return ::epoll_ctl(epfd_, EPOLL_CTL_ADD, fd, &ev_);
#else 
    int num = 0;
    EV_SET(&ev_[num++], fd, EVFILT_READ, EV_ADD|EV_ENABLE, 0, 0, &fd);
    //EV_SET(&ev_[num++], fd, EVFILT_WRITE, EV_ADD|EV_ENABLE, 0, 0, &fd);
    return num;
    //return kevent(epfd_, ev_, n, NULL, 0, NULL);
#endif
}

int Epoll::EpollDel(int fd)
{
#ifndef __APPLE__
    ev_.data.fd = fd;
    ::epoll_ctl(epfd_, EPOLL_CTL_DEL, fd, NULL);
#else
    int num = 0;
    EV_SET(&ev_[num++], fd, EVFILT_READ, EV_DELETE, 0, 0, &fd);
    //EV_SET(&ev_[num++], fd, EVFILT_WRITE, EV_DELETE, 0, 0, &fd);
#endif
    // erase: 0 不存在元素 1 存在元素
    return listeners_.erase(fd) - 1;
}

bool Epoll::EpoolQuit()
{
    epoll_loop_ = false;
    return true;
}

int Epoll::EpollLoop()
{
    struct timespec timeout;
    timeout.tv_sec = 1;
    timeout.tv_nsec = 0;

    while (epoll_loop_) {
        
#ifndef __APPLE__
        int nfds = ::epoll_wait(epfd_, events_, MAXEVENTS, timeout.tv_sec*1000 + timeout.tv_nsec/1000000);
#else
        int nfds = kevent(epfd_, ev_, 2, activeEvs_, MAXEVENTS, &timeout);
#endif

        if (nfds == -1) {
            ::perror("loop");
            //::exit(1);
        }

        if (nfds == 0) {
          // std::cout << "Epoll time out" << std::endl;
          return 0;
        }

        for (int i = 0; i < nfds; i++) {
            // 有消息可读取
#ifndef __APPLE__
            if (events_[i].events & EPOLLIN) {
                // 在map中寻找对应的回调函数
                auto handle_it = listeners_.find(events_[i].data.fd);
                if (handle_it != listeners_.end()) {
                    handle_it->second();
                } else {
                    std::cout << "can not find the fd:" << events_[i].data.fd << std::endl;
                }
            }
#else
            int fd = (int)(intptr_t)activeEvs_[i].udata;
            int events = activeEvs_[i].filter;
            if (events == EVFILT_READ) {
                std::cout << "read data from fd:" << fd << std::endl;
                auto handle_it = listeners_.find(fd);
                if (handle_it != listeners_.end()) {
                    handle_it->second();
                } else {
                    std::cout << "can not find the fd:" << fd << std::endl;
                }
            } else if (events == EVFILT_WRITE) {
                std::cout << "write data to fd:" << fd << std::endl;
            } else {
                assert("unknown event");
            }
#endif
        }
    }

    return 0;
}
