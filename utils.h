#pragma once

#include <vector>
#include <fstream>
#include <string>
#include <iostream>
#include <ctime>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <sys/stat.h>

#define MAX_RATE_RECORD 102400 // 100K 循环

namespace Utils {
    void getFiles(std::string path, std::vector<std::string>& files);
    std::string getCurrentTime0();
    template<typename T> bool WriteFile(std::string path, T data);
}

template<typename T>
bool Utils::WriteFile(std::string path, T data) {
    int size = 0;
    struct stat statbuf;
    if (stat(path.c_str(), &statbuf) == 0) {
        size = statbuf.st_size;
    }

	std::ofstream out_file;
    if (size > MAX_RATE_RECORD) { //大于MAX_SIZE清零后重新写入
		out_file.open(path, std::ios::out | std::ios::trunc);
    } else {
		out_file.open(path, std::ios::out | std::ios::app);
	}

	if (!out_file.is_open()) {
		return false;
	}

	out_file << Utils::getCurrentTime0() << "\t" << data << std::endl;
	out_file.close();
	return true;
}