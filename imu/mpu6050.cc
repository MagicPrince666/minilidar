#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>

#include "mpu6050.h"
#include "interface.h"

Mpu6050::Mpu6050(std::string device) 
: device_(device) {
    // mpu6050_fd_ = fopen(device.c_str(),"r+");
    // if (NULL == mpu6050_fd_) {
    //     std::cout << RED << device << " not exists!" << std::endl;
	// 	exit(-1);
	// }
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

std::vector<std::string> accel = {
    {"in_accel_x_raw"},
    {"in_accel_y_raw"},
    {"in_accel_z_raw"}
};

std::vector<std::string> anglvel = {
    {"in_anglvel_x_raw"},
    {"in_anglvel_y_raw"},
    {"in_anglvel_z_raw"}
};

//得到陀螺仪值(原始值)
//gx,gy,gz:陀螺仪x,y,z轴的原始读数(带符号)
//返回值:0,成功
//    其他,错误代码
bool Mpu6050::MpuGetGyroscope()
{
    for (std::vector<std::string>::iterator iter = accel.begin(); iter != accel.end(); iter++) {
        std::string buf = ReadFileIntoString(device_ + *iter);
        std::cout << GREEN << *iter << " = " << buf << std::endl;
	}

    return true;
}
 
//得到加速度值(原始值)
//ax,ay,az:陀螺仪x,y,z轴的原始读数(带符号)
//返回值:0,成功
//    其他,错误代码
bool Mpu6050::MpuGetAccelerometer()
{
    for (std::vector<std::string>::iterator iter = anglvel.begin(); iter != anglvel.end(); iter++) {
        std::string buf = ReadFileIntoString(device_ + *iter);
        std::cout << GREEN << *iter << " = " << buf << std::endl;
	}
    return true;
}

