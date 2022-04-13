#pragma once

#include <inttypes.h>
#include <string>
#include "xepoll.h"

class Uart
{
public:
    Uart(std::string device = "/dev/ttyUSB0");
    ~Uart();
    int sendData(const char* bufout, int size);
    int RecvData(char* bufin);
    bool UartLoop();
    bool UartRead();

private:
    int uart_fd_ = -1;
    int OpenSerial(const char *cSerialName, int Bitrate);
};
