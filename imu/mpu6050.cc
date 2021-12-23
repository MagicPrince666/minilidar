#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <map>
#include <sys/stat.h>

#include "mpu6050.h"
#include "interface.h"

Mpu6050::Mpu6050(std::string device) 
: device_(device) {
    struct stat buffer;
    if(stat(device.c_str(), &buffer) != 0) {
        std::cout << RED <<"Device " << device <<" not extis\n";
        exit(-1);
    }
}

Mpu6050::~Mpu6050() {
    std::cout << BOLDGREEN << "Close mpu6050 device!" << std::endl;
}

std::string Mpu6050::ReadFileIntoString(const std::string& path) {
    std::ifstream input_file(path);
    if (!input_file.is_open()) {
        return "";
    }
    return std::string((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());
}

std::unordered_map<std::string, int> accel = {
    {"in_accel_x_raw", 0},
    {"in_accel_y_raw", 0},
    {"in_accel_z_raw", 0}
};

std::unordered_map<std::string, int> anglvel = {
    {"in_anglvel_x_raw", 0},
    {"in_anglvel_y_raw", 0},
    {"in_anglvel_z_raw", 0}
};

//得到陀螺仪值(原始值)
//gx,gy,gz:陀螺仪x,y,z轴的原始读数(带符号)
//返回值:0,成功
//    其他,错误代码
bool Mpu6050::MpuGetGyroscope(int &gx, int &gy, int &gz)
{
    for (std::unordered_map<std::string, int>::iterator iter = accel.begin(); iter != accel.end(); iter++) {
        std::string buf = ReadFileIntoString(device_ + iter->first);
        iter->second = atoi(buf.c_str());
        std::cout << GREEN << iter->first << " = " << iter->second << std::endl;
	}
    gx = accel["in_accel_x_raw"];
    gy = accel["in_accel_y_raw"];
    gz = accel["in_accel_z_raw"];
    return true;
}
 
//得到加速度值(原始值)
//ax,ay,az:陀螺仪x,y,z轴的原始读数(带符号)
//返回值:0,成功
//    其他,错误代码
bool Mpu6050::MpuGetAccelerometer(int &ax, int &ay, int &az)
{
    for (std::unordered_map<std::string, int>::iterator iter = anglvel.begin(); iter != anglvel.end(); iter++) {
        std::string buf = ReadFileIntoString(device_ + iter->first);
        iter->second = atoi(buf.c_str());
        std::cout << GREEN << iter->first << " = " << iter->second << std::endl;
	}
    ax = anglvel["in_anglvel_x_raw"];
    ay = anglvel["in_anglvel_y_raw"];
    az = anglvel["in_anglvel_z_raw"];
    return true;
}

