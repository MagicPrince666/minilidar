#pragma once

#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <iostream>

class Vl53l0x {
public:
    Vl53l0x(std::string device = "/sys/bus/iio/devices/iio:device0/in_distance_raw");
    ~Vl53l0x();
    int GetDistance();

private:
    FILE* vl53l0x_fd_{nullptr};
    std::string device_;
    std::string ReadFileIntoString(const std::string& path);
};
