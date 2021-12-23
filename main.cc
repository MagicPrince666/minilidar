#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <stdexcept>
#include <vector>
#include <chrono>
#include <codecvt>
#include <string>
#include <iomanip>
#include <algorithm>
#include <sstream>
#include <signal.h>
#include <execinfo.h>

#include "spdlog/spdlog.h"
#include "spdlog/cfg/env.h"  // support for loading levels from the environment variable
#include "spdlog/fmt/ostr.h" // support for user defined types

#include "common.h"
#include "uart.h"
#include "xepoll.h"
#include "inotify.h"
#include "interface.h"
#include "vl53l0x.h"
#include "mpu6050.h"

bool loop = true;

std::string CurrentTime()
{
	std::stringstream ss;
	std::string str = "%Y-%m-%d %H-%M-%S";
	std::chrono::system_clock::time_point a = std::chrono::system_clock::now();      //时间点可以做差
	time_t t1 = std::chrono::system_clock::to_time_t(a);				  //time_t可以格式化
	ss << std::put_time(localtime(&t1), str.c_str());
	std::string str1 = ss.str();
	return str1;
}

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
    // signal(signum, SIG_DFL); /* 还原默认的信号处理handler */

    exit(1);
}

static void sigint_handler(int sig)
{
	std::cout << "--- quit the loop! ---" << std::endl;
	loop = false;
}

int main(int argc, char* argv[])
{
    signal(SIGPIPE, _signal_handler);  // SIGPIPE，管道破裂。
    signal(SIGSEGV, _signal_handler);  // SIGSEGV，非法内存访问
    signal(SIGFPE, _signal_handler);  // SIGFPE，数学相关的异常，如被0除，浮点溢出，等等
    signal(SIGABRT, _signal_handler);  // SIGABRT，由调用abort函数产生，进程非正常退出
    signal(SIGINT, sigint_handler);//信号处理

    spdlog::info("Welcome to spdlog version {}.{}.{}  !", SPDLOG_VER_MAJOR, SPDLOG_VER_MINOR, SPDLOG_VER_PATCH);

	std::string title = "Huang liquan ";
	title += CurrentTime();

	LcdRgb lcd(0);
	lcd.fill_screen_solid(0x0000ff);
	lcd.fb_put_string(30, 0, title.c_str(), title.size(), RGB_GOLDEN, true, title.size());

    Vl53l0x vl53l0x;
    Mpu6050 mpu6050;
    std::string distance;

    while (loop) {
        distance = vl53l0x.GetDistance();
        lcd.fb_put_string(30, 20, distance.c_str(), distance.size(), RGB_VERMILION, true, distance.size());

        int gx,gy,gz;
        int ax,ay,az;
        mpu6050.MpuGetGyroscope(gx, gy, gz);
        mpu6050.MpuGetAccelerometer(ax, ay, az);
        lcd.fb_put_string(30, 40, gx, 5, RGB_GOLDEN, true, 5);
        lcd.fb_put_string(30, 60, gy, 5, RGB_GOLDEN, true, 5);
        lcd.fb_put_string(30, 80, gz, 5, RGB_GOLDEN, true, 5);

        lcd.fb_put_string(100, 40, ax, 5, RGB_GOLDEN, true, 5);
        lcd.fb_put_string(100, 60, ay, 5, RGB_GOLDEN, true, 5);
        lcd.fb_put_string(100, 80, az, 5, RGB_GOLDEN, true, 5);
        usleep(500000);
    }

    // Xepoll xepoll;//初始化事件模型

    // Uart serial(&xepoll, "/dev/ttyUSB0");

    // //初始化文件监控事件并加入事件列表
    // Inotify inotify(&xepoll, "/tmp/text");

    // return xepoll.loop();//等待事件触发

    return 0;
}
