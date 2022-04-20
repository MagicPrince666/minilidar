#include <iostream>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include "xepoll.h"
#include "socket.h"

Socket::Socket() {
    port = 99;
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof addr);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;
    int r = ::bind(listenfd,(struct sockaddr *)&addr, sizeof(struct sockaddr));
    r = listen(listenfd, 20);
    printf("fd %d listening at %d\n", listenfd, port);
    MY_EPOLL->EpollAdd(listenfd, std::bind(&Socket::handleRead, this));
}

Socket::~Socket() {}


void Socket::handleAccept() {
    struct sockaddr_in raddr;
    socklen_t rsz = sizeof(raddr);
    int cfd = accept(listenfd,(struct sockaddr *)&raddr,&rsz);
    //exit_if(cfd<0, "accept failed");
    sockaddr_in peer;
    socklen_t alen = sizeof(peer);
    int r = getpeername(cfd, (sockaddr*)&peer, &alen);
    //exit_if(r<0, "getpeername failed");
    printf("accept a connection from %s\n", inet_ntoa(raddr.sin_addr));
    //setNonBlock(cfd);
    // updateEvents(cfd, 1|2, false);
}

void Socket::handleRead() {
    char buf[4096];
    int n = 0;
    while ((n=::read(listenfd, buf, sizeof buf)) > 0) {
        printf("read %d bytes\n", n);
        int r = ::write(listenfd, buf, n); //写出读取的数据
        //实际应用中，写出数据可能会返回EAGAIN，此时应当监听可写事件，当可写时再把数据写出
        //exit_if(r<=0, "write error");
    }
    if (n < 0 && (errno == EAGAIN || errno == EWOULDBLOCK))
        return;
    //exit_if(n<0, "read error"); //实际应用中，n<0应当检查各类错误，如EINTR
    printf("fd %d closed\n", listenfd);
    close(listenfd);
}