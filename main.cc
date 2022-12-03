#include <iostream>
#include <cstdio>
#include <string>
#include <signal.h>

#include "spdlog/spdlog.h"
#include "spdlog/cfg/env.h"  // support for loading levels from the environment variable
#include "spdlog/fmt/ostr.h" // support for user defined types

#include "xepoll.h"
#include "udp_server.h"
#include "utils.h"
// #include "uart.h"
// #include "timerfd.h"
// #include "inotify.h"
// #include "gpio_key.h"
// #include "interface.h"
#include "common.h"

#define BACKTRACE_DEBUG 0

#if BACKTRACE_DEBUG
#include <execinfo.h>

#define PRINT_SIZE_ 100

static void _signal_handler(int signum)
{
    void *array[PRINT_SIZE_];
    char **strings;

    size_t size = backtrace(array, PRINT_SIZE_);
    strings = backtrace_symbols(array, size);

    if (strings == nullptr) {
	   fprintf(stderr, "backtrace_symbols");
	   exit(EXIT_FAILURE);
    }

    switch(signum) {
        case SIGSEGV:
        fprintf(stderr, "widebright received SIGSEGV! Stack trace:\n");
        break;

        case SIGPIPE:
        fprintf(stderr, "widebright received SIGPIPE! Stack trace:\n");
        break;

        case SIGFPE:
        fprintf(stderr, "widebright received SIGFPE! Stack trace:\n");
        break;

        case SIGABRT:
        fprintf(stderr, "widebright received SIGABRT! Stack trace:\n");
        break;

        default:
        break;
    }

    for (size_t i = 0; i < size; i++) {
        fprintf(stderr, "%d %s \n", i, strings[i]);
    }

    free(strings);
    signal(signum, SIG_DFL); /* 还原默认的信号处理handler */

    exit(1);
}
#endif

static void sigint_handler(int sig)
{
	std::cout << "--- quit the loop! ---" << std::endl;
    MY_EPOLL.EpoolQuit();
    exit(0);
}

int main(int argc, char* argv[])
{
#if BACKTRACE_DEBUG
    signal(SIGPIPE, _signal_handler);  // SIGPIPE，管道破裂。
    signal(SIGSEGV, _signal_handler);  // SIGSEGV，非法内存访问
    signal(SIGFPE, _signal_handler);  // SIGFPE，数学相关的异常，如被0除，浮点溢出，等等
    signal(SIGABRT, _signal_handler);  // SIGABRT，由调用abort函数产生，进程非正常退出
#endif

    signal(SIGINT, sigint_handler);//中断信号处理

    spdlog::info("Welcome to spdlog version {}.{}.{}  !", SPDLOG_VER_MAJOR, SPDLOG_VER_MINOR, SPDLOG_VER_PATCH);

    LcdRgb lcd;
    lcd.Init();
    lcd.FillScreenSolid(RGB_BLUE);

    while(1) {
        sleep(1);
    }

    MY_EPOLL.EpollLoop();//等待事件触发

    return 0;

}
