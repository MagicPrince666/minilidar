#include <fcntl.h>
#include <unistd.h>

#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <string>

#include "xepoll.h"
Epoll *Epoll::instance_ = nullptr;

Epoll::Epoll(void) { epfd_ = ::epoll_create(EPOLL_FD_SETSIZE); }

Epoll::~Epoll(void)
{
    if (epfd_ > 0) {
        ::close(epfd_);
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
    ev_.data.fd    = fd;
    ev_.events     = EPOLLIN;

    // 设置为非阻塞
    int sta = ::fcntl(fd, F_GETFD, 0) | O_NONBLOCK;
    if (::fcntl(fd, F_SETFL, sta) < 0) {
        return -1;
    }

    return ::epoll_ctl(epfd_, EPOLL_CTL_ADD, fd, &ev_);
}

int Epoll::EpollDel(int fd)
{
    ev_.data.fd = fd;
    ::epoll_ctl(epfd_, EPOLL_CTL_DEL, fd, NULL);
    // erase: 0 不存在元素 1 存在元素
    return listeners_.erase(fd) - 1;
}

bool Epoll::EpoolQuit()
{
    epoll_loop_ = false;
    return false;
}

int Epoll::EpollLoop()
{
    while (epoll_loop_) {
        nfds_ = ::epoll_wait(epfd_, events_, MAXEVENTS, 1000);

        if (nfds_ == -1) {
            ::perror("loop");
            ::exit(1);
        }

        // if (nfds_ == 0) {
        //   std::cout << "Epoll time out" << std::endl;
        // }

        for (int i = 0; i < nfds_; i++) {
            // 有消息可读取
            if (events_[i].events & EPOLLIN) {
                // 在map中寻找对应的回调函数
                auto handle_it = listeners_.find(events_[i].data.fd);
                if (handle_it != listeners_.end()) {
                    handle_it->second();
                } else {
                    std::cout << "can not find the fd:" << events_[i].data.fd << std::endl;
                }
            }
        }
    }

    return 0;
}
