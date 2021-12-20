#pragma once

#include <iostream>
#include <string>

class Mpu6050 {
public:
    Mpu6050(std::string device = "/sys/bus/iio/devices/iio:device1/in_distance_raw");
    ~Mpu6050();
    bool MpuGetGyroscope(short *gx,short *gy,short *gz);
    bool MpuGetAccelerometer(short *ax,short *ay,short *az);

private:
    FILE* mpu6050_fd_{nullptr};
    std::string device_;
    std::string ReadFileIntoString(const std::string& path);
};
