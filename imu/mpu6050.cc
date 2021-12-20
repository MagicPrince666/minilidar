#include <iostream>
#include <fstream>
#include <sstream>

#include "mpu6050.h"
#include "interface.h"

Mpu6050::Mpu6050(std::string device) 
: device_(device) {
    mpu6050_fd_ = fopen(device.c_str(),"r+");
    if (NULL == mpu6050_fd_) {
        std::cout << RED << device << " not exists!" << std::endl;
		exit(-1);
	}
}

Mpu6050::~Mpu6050() {
    fclose(mpu6050_fd_);
    std::cout << BOLDGREEN << "Close device!" << std::endl;
}

std::string Mpu6050::ReadFileIntoString(const std::string& path) {
    std::ifstream input_file(path);
    if (!input_file.is_open()) {
        return "";
    }
    return std::string((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());
}

//得到陀螺仪值(原始值)
//gx,gy,gz:陀螺仪x,y,z轴的原始读数(带符号)
//返回值:0,成功
//    其他,错误代码
bool Mpu6050::MpuGetGyroscope(short *gx,short *gy,short *gz)
{
    std::string bus = "/sys/bus/iio/devices/iio:device1/";
    std::string in_accel_x_raw = "in_accel_x_raw";
    std::string in_accel_y_raw = "in_accel_y_raw";
    std::string in_accel_z_raw = "in_accel_z_raw";

    std::string accel_x = ReadFileIntoString(bus + in_accel_x_raw);
    std::string accel_y = ReadFileIntoString(bus + in_accel_y_raw);
    std::string accel_z = ReadFileIntoString(bus + in_accel_z_raw);

    return true;
}
 
//得到加速度值(原始值)
//ax,ay,az:陀螺仪x,y,z轴的原始读数(带符号)
//返回值:0,成功
//    其他,错误代码
bool Mpu6050::MpuGetAccelerometer(short *ax,short *ay,short *az)
{
    std::string bus = "/sys/bus/iio/devices/iio:device1/";
    std::string in_anglvel_x_raw = "in_anglvel_x_raw";
    std::string in_anglvel_y_raw = "in_anglvel_y_raw";
    std::string in_anglvel_z_raw = "in_anglvel_z_raw";

    std::string anglvel_x = ReadFileIntoString(bus + in_anglvel_x_raw);
    std::string anglvel_y = ReadFileIntoString(bus + in_anglvel_y_raw);
    std::string anglvel_z = ReadFileIntoString(bus + in_anglvel_z_raw);
    return true;
}

