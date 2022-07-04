#include <iostream>
#include <new>
#include <sys/socket.h>
#include <sys/event.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include "kqueue.h"

#define exit_if(r, ...) if(r) {printf(__VA_ARGS__); printf("error no: %d error msg %s\n", errno, strerror(errno)); exit(1);}

Kqueue *Kqueue::instance_ = nullptr;

Kqueue::Kqueue(void) {
    epollfd = kqueue();
    exit_if(epollfd < 0, "epoll_create failed");
}

Kqueue::~Kqueue(void)
{
    if (!instance_) {
        delete instance_;
    }
}

Kqueue *Kqueue::Instance()
{
    if (!instance_) {
        instance_ = new (std::nothrow)Kqueue();
    }
    return instance_;
}

void Kqueue::setNonBlock(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    exit_if(flags<0, "fcntl failed");
    int r = fcntl(fd, F_SETFL, flags | O_NONBLOCK);
    exit_if(r<0, "fcntl failed");
}

void Kqueue::updateEvents(int fd, int events, bool modify) {
    struct kevent ev[2];
    int n = 0;
    if (events & kReadEvent) {
        EV_SET(&ev[n++], fd, EVFILT_READ, EV_ADD|EV_ENABLE, 0, 0, (void*)(intptr_t)fd);
    } else if (modify){
        EV_SET(&ev[n++], fd, EVFILT_READ, EV_DELETE, 0, 0, (void*)(intptr_t)fd);
    }
    if (events & kWriteEvent) {
        EV_SET(&ev[n++], fd, EVFILT_WRITE, EV_ADD|EV_ENABLE, 0, 0, (void*)(intptr_t)fd);
    } else if (modify){
        EV_SET(&ev[n++], fd, EVFILT_WRITE, EV_DELETE, 0, 0, (void*)(intptr_t)fd);
    }
    printf("%s fd %d events read %d write %d\n",
           modify ? "mod" : "add", fd, events & kReadEvent, events & kWriteEvent);
    int r = kevent(epollfd, ev, n, NULL, 0, NULL);
    exit_if(r, "kevent failed ");
}

void Kqueue::handleAccept(int fd) {
    struct sockaddr_in raddr;
    socklen_t rsz = sizeof(raddr);
    int cfd = accept(fd,(struct sockaddr *)&raddr,&rsz);
    exit_if(cfd<0, "accept failed");
    sockaddr_in peer;
    socklen_t alen = sizeof(peer);
    int r = getpeername(cfd, (sockaddr*)&peer, &alen);
    exit_if(r<0, "getpeername failed");
    printf("accept a connection from %s\n", inet_ntoa(raddr.sin_addr));
    setNonBlock(cfd);
    updateEvents(cfd, kReadEvent|kWriteEvent, false);
}

void Kqueue::handleRead(int fd) {
    char buf[4096];
    int n = 0;
    while ((n=::read(fd, buf, sizeof buf)) > 0) {
        printf("read %d bytes\n", n);
        int r = ::write(fd, buf, n); //写出读取的数据
        //实际应用中，写出数据可能会返回EAGAIN，此时应当监听可写事件，当可写时再把数据写出
        exit_if(r<=0, "write error");
    }
    if (n<0 && (errno == EAGAIN || errno == EWOULDBLOCK))
        return;
    exit_if(n<0, "read error"); //实际应用中，n<0应当检查各类错误，如EINTR
    printf("fd %d closed\n", fd);
    close(fd);
}

void Kqueue::handleWrite(int fd) {
    //实际应用应当实现可写时写出数据，无数据可写才关闭可写事件
    updateEvents(fd, kReadEvent, true);
}

void Kqueue::loop_once(int lfd, int waitms) {
    struct timespec timeout;
    timeout.tv_sec = waitms / 1000;
    timeout.tv_nsec = (waitms % 1000) * 1000 * 1000;
    const int kMaxEvents = 20;
    struct kevent activeEvs[kMaxEvents];
    int n = kevent(epollfd, NULL, 0, activeEvs, kMaxEvents, &timeout);
    printf("epoll_wait return %d\n", n);
    for (int i = 0; i < n; i ++) {
        int fd = (int)(intptr_t)activeEvs[i].udata;
        int events = activeEvs[i].filter;
        if (events == EVFILT_READ) {
            if (fd == lfd) {
                handleAccept(fd);
            } else {
                handleRead(fd);
            }
        } else if (events == EVFILT_WRITE) {
            handleWrite(fd);
        } else {
            exit_if(1, "unknown event");
        }
    }
}

