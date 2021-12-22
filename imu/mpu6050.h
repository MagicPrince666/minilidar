#pragma once

#include <iostream>
#include <string>
#include <vector>

class Mpu6050 {
public:
    Mpu6050(std::string device = "/sys/bus/iio/devices/iio:device1/");
    ~Mpu6050();
    bool MpuGetGyroscope();
    bool MpuGetAccelerometer();

private:
    FILE* mpu6050_fd_{nullptr};
    std::string device_ = "/sys/bus/iio/devices/iio:device1/";
    std::string ReadFileIntoString(const std::string& path);
};
