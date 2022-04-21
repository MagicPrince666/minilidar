#pragma once

#include <sys/socket.h>
#include <netinet/in.h>

class Socket
{
public:
    Socket();
    ~Socket();
    void handleAccept();
    void handleRead();
private:
    int listenfd_ = -1;
    uint32_t port = 99;
};