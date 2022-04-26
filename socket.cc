#include <iostream>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "xepoll.h"
#include "socket.h"

Socket::Socket() {
    listenfd_ = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof addr);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port_);
    addr.sin_addr.s_addr = INADDR_ANY;

    int r = ::bind(listenfd_, (struct sockaddr *)&addr, sizeof(struct sockaddr));
    assert(r >= 0);

    r = listen(listenfd_, 20);
    assert(r >= 0);

    printf("fd %d listening at %d\n", listenfd_, port_);

    MY_EPOLL->EpollAdd(listenfd_, std::bind(&Socket::handleRead, this));
}

Socket::~Socket() {
    MY_EPOLL->EpollDel(listenfd_);
    if(listenfd_ > 0) {
        close(listenfd_);
    }
}


void Socket::handleAccept() {
    struct sockaddr_in raddr;
    socklen_t rsz = sizeof(raddr);
    int cfd = accept(listenfd_, (struct sockaddr *)&raddr, &rsz);
    assert(cfd >= 0);
    sockaddr_in peer;
    socklen_t alen = sizeof(peer);
    int r = getpeername(cfd, (sockaddr*)&peer, &alen);
    assert(r >= 0);
    printf("accept a connection from %s\n", inet_ntoa(raddr.sin_addr));

    MY_EPOLL->EpollAdd(cfd, std::bind(&Socket::handleRead, this));
}

void Socket::handleRead() {
    char buf[4096];
    int n = 0;
    while ((n=::read(listenfd_, buf, sizeof buf)) > 0) {
        std::cout <<"read " << n << " bytes" << std::endl;
        int r = ::write(listenfd_, buf, n); //写出读取的数据
        //实际应用中，写出数据可能会返回EAGAIN，此时应当监听可写事件，当可写时再把数据写出
        assert(r >= 0);
    }
    if (n < 0 && (errno == EAGAIN || errno == EWOULDBLOCK))
        return;
    //实际应用中，n<0应当检查各类错误，如EINTR
}

void Socket::handleWrite() {
    //实际应用应当实现可写时写出数据，无数据可写才关闭可写事件
    // updateEvents(listenfd_, kReadEvent, true);
}
