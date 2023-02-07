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
    device_dir_ = Utils::ScanIioDevice(dev);
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
