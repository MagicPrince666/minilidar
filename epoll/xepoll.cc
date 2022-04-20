#include <fcntl.h>
#include <unistd.h>

#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <string>

#include "xepoll.h"
Epoll *Epoll::instance_ = nullptr;

Epoll::Epoll(void) {
#ifdef __APPLE__
    epfd_ = kqueue();
#else
    epfd_ = ::epoll_create(EPOLL_FD_SETSIZE);
#endif
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
    int n = 0;
    bool modify = false;
    if (events_ & kReadEvent) {
        EV_SET(&ev_[n++], fd, EVFILT_READ, EV_ADD|EV_ENABLE, 0, 0, (void*)(intptr_t)fd);
    } else if (modify){
        EV_SET(&ev_[n++], fd, EVFILT_READ, EV_DELETE, 0, 0, (void*)(intptr_t)fd);
    }
    if (events_ & kWriteEvent) {
        EV_SET(&ev_[n++], fd, EVFILT_WRITE, EV_ADD|EV_ENABLE, 0, 0, (void*)(intptr_t)fd);
    } else if (modify){
        EV_SET(&ev_[n++], fd, EVFILT_WRITE, EV_DELETE, 0, 0, (void*)(intptr_t)fd);
    }
    printf("%s fd %d events read %d write %d\n",
           modify ? "mod" : "add", fd, events_ & kReadEvent, events_ & kWriteEvent);
    return kevent(epfd_, ev_, n, NULL, 0, NULL);
#endif
}

int Epoll::EpollDel(int fd)
{
#ifndef __APPLE__
    ev_.data.fd = fd;
    ::epoll_ctl(epfd_, EPOLL_CTL_DEL, fd, NULL);
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
        nfds_ = ::epoll_wait(epfd_, events_, MAXEVENTS, 1000);
#else
        nfds_ = kevent(epfd_, NULL, 0, activeEvs_, MAXEVENTS, &timeout);
#endif

        if (nfds_ == -1) {
            ::perror("loop");
            //::exit(1);
        }

        // if (nfds_ == 0) {
        //   std::cout << "Epoll time out" << std::endl;
        // }

        for (int i = 0; i < nfds_; i++) {
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
                auto handle_it = listeners_.find(fd);
                if (handle_it != listeners_.end()) {
                    handle_it->second();
                } else {
                    std::cout << "can not find the fd:" << fd << std::endl;
                }
            } else if (events == EVFILT_WRITE) {
            } else {
                assert("unknown event");
            }
#endif
        }
    }

    return 0;
}
