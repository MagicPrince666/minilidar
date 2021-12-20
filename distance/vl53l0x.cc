#include <iostream>
#include <fstream>
#include <sstream>

#include "vl53l0x.h"
#include "interface.h"

Vl53l0x::Vl53l0x(std::string device) 
: device_(device) {
    vl53l0x_fd_ = fopen(device.c_str(),"r+");
    if (NULL == vl53l0x_fd_) {
        std::cout << RED << device << " not exists!" << std::endl;
		exit(-1);
	}
}

Vl53l0x::~Vl53l0x() {
    fclose(vl53l0x_fd_);
    std::cout << BOLDGREEN << "Close device!" << std::endl;
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
	// char buf[1024];

	// memset(buf, 0, sizeof(buf));

	// fseek(vl53l0x_fd_, 0, SEEK_SET);
	// int nBytes = fread(buf, 1, sizeof(buf)-1, vl53l0x_fd_);
	// if (-1 == nBytes) {
	// 	return -1;
	// }

    std::string buf = ReadFileIntoString(device_);

    std::cout << GREEN << "Distance:" << buf << std::endl;

    return 0;
}