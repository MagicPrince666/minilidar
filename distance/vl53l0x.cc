#include <iostream>
#include <fstream>
#include <sstream>
#include <assert.h>

#include "vl53l0x.h"
#include "interface.h"

Vl53l0x::Vl53l0x(std::string device) 
: device_(device) {
    vl53l0x_fd_ = fopen(device.c_str(),"r+");
    if (nullptr == vl53l0x_fd_) {
        std::cout << RED << device << " not exists!" << std::endl;
		assert(vl53l0x_fd_ == nullptr);
	}
}

Vl53l0x::~Vl53l0x() {
    if(vl53l0x_fd_ != nullptr) {
        fclose(vl53l0x_fd_);
    }
    std::cout << BOLDGREEN << "Close vl53l0x device!" << std::endl;
}

std::string Vl53l0x::ReadFileIntoString(const std::string& path) {
    std::ifstream input_file(path);
    if (!input_file.is_open()) {
        return "";
    }
    return std::string((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());
}

int Vl53l0x::GetDistance()
{
    std::string buf = ReadFileIntoString(device_);
    int val = atoi(buf.c_str());

    return val;
}