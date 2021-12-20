#include <iostream>
#include <string>

#include "uart.h"
#include "xepoll.h"
#include "inotify.h"
#include "interface.h"

int main(int argc, char* argv[])
{
    Xepoll xepoll;//初始化事件模型

    Uart serial(&xepoll, "/dev/ttyUSB0");

    //初始化文件监控事件并加入事件列表
    Inotify inotify(&xepoll, "/tmp/text");

    return xepoll.loop();//等待事件触发
}