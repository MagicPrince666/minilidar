#include <iostream>
#include <linux/input.h>
#include <linux/types.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <iomanip>
#include <assert.h>

#include "ultrasonic.h"
#include "utils.h"

Ultrasonic::Ultrasonic(std::string dev)
{
    device_dir_ = ScanIioDevice(dev);
    assert(device_dir_ != "");
    std::cout << BOLDGREEN << "Iio bus path " << device_dir_ << std::endl;
}

Ultrasonic::~Ultrasonic(void)
{
    std::cout << BOLDGREEN << "Close srf04 device!" << std::endl;
}

int Ultrasonic::Srf04Distance()
{
    std::string distance_str = device_dir_ + "in_distance_raw";
    std::string buf = Utils::ReadFileIntoString(distance_str);
    int distance = atoi(buf.c_str());

    return distance;
}

std::string Ultrasonic::ScanIioDevice(std::string name) {
    std::string device = "";
    if(!name.empty()) { //扫描设备
        for(int i = 0; i < 10; i++) {
            std::string filename = "/sys/bus/iio/devices/iio:device" + std::to_string(i) + "/name";
            // std::cout << "iio bus name" << filename << std::endl;
            struct stat buffer;   
            if(stat(filename.c_str(), &buffer) == 0) {
                std::ifstream iio_name;
                iio_name.open(filename, std::ios::in);
                char buff[64] = {0};
                iio_name.read(buff, sizeof(buff));
                std::string dev_name(buff);

                if (dev_name.find(name) != std::string::npos) {
                    // std::cout << "find " << name << " device\n";
                    device = "/sys/bus/iio/devices/iio:device" + std::to_string(i) +"/";
                    break;
                }
            } else {
                // 搜索完了，没有找到对应设备
                std::cout << "iio bus scan end\n";
                break;
            }
        }
    }

    return device;
}
