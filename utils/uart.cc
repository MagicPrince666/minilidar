#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <string.h>
#include <assert.h>

#include "uart.h"

#include <iostream>

#define BAUDRATE     115200

#define DEBUG_UART 0

#define DATA_LEN    0xFF

Uart::Uart(std::string device)
{
    std::cout << "uart init " << device << std::endl;

    uart_fd_ = OpenSerial(device.c_str(), BAUDRATE);
    if(uart_fd_ < 0) {
        std::cout <<"can\'t open " << device <<" !" << std::endl;
        assert(uart_fd_ >= 0);
    } else tcflush(uart_fd_, TCIOFLUSH);//清空串口输入输出缓存

    std::string cmd = "WLACC\r\n";
    sendData(cmd.c_str(), cmd.size());

    // 绑定回调函数
    if (uart_fd_ > 0) {
        std::cout << "Bind epoll" << std::endl;
        MY_EPOLL.EpollAdd(uart_fd_, std::bind(&Uart::UartLoop, this));
    }
}

Uart::~Uart(void)
{
    std::cout << "uart deinit" << std::endl;

    if(uart_fd_ > 0) {
        MY_EPOLL.EpollDel(uart_fd_);
        close(uart_fd_);
    }
}

int Uart::OpenSerial(const char *cSerialName, int Bitrate)
{
    int iFd;

    struct termios opt;

    iFd = open(cSerialName, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if(iFd < 0) {
        perror(cSerialName);
        return -1;
    }

    tcgetattr(iFd, &opt);

	switch(Bitrate) {
		case 115200:
		cfsetispeed(&opt, B115200);
		cfsetospeed(&opt, B115200);
		break;
		default :
		printf("error uart bitrate!\n");
		exit(0);
		break;
	}

    /*
     * raw mode
     */
    opt.c_lflag   &=   ~(ECHO   |   ICANON   |   IEXTEN   |   ISIG);
    opt.c_iflag   &=   ~(BRKINT   |   ICRNL   |   INPCK   |   ISTRIP   |   IXON);
    opt.c_oflag   &=   ~(OPOST);
    opt.c_cflag   &=   ~(CSIZE   |   PARENB);
    opt.c_cflag   |=   CS8;

    /*
     * 'DATA_LEN' bytes can be read by serial
     */
    opt.c_cc[VMIN]   =   DATA_LEN;
    opt.c_cc[VTIME]  =   150;

    if (tcsetattr(iFd,   TCSANOW,   &opt)<0) {
        return   -1;
    }

    return iFd;
}

int Uart::sendData(const char* bufout, int size)
{
    int len = write(uart_fd_, bufout, size);
    if(len != size) {
        std::cout << "Send Error" << std::endl;
    }
    return len;
}

int Uart::RecvData(char* bufin)
{
    int len = read(uart_fd_, bufin, 1024);
    return len;
}

bool Uart::UartLoop()
{
    char* buf = new (std::nothrow)char[1024];
    while(true) {
        int len = RecvData(buf);
        if(len > 0) {
            buf[len] = 0;
            std::cout << buf << std::endl;
        }
        usleep(100000);
    }
    delete[] buf;
    return true;
}

bool Uart::UartRead()
{
    char* buf = new (std::nothrow)char[1024];
    int len = read(uart_fd_, buf, 1024);
    buf[len] = 0;
    std::cout << buf << std::endl;
    delete[] buf;
    return true;
}