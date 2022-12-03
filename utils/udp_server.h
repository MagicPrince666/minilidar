#pragma once

#include <inttypes.h>

class UdpServer
{
public:
    UdpServer();
    ~UdpServer();

    bool HandleSocket();

private:
    int listenfd_ = -1;
    uint32_t port_ = 5555;
};