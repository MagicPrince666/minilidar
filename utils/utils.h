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

#define MAX_RATE_RECORD 1024 // 100K 循环

namespace Utils {
    void getFiles(std::string path, std::vector<std::string>& files);
    std::string getCurrentTime0();
    std::string ReadFileIntoString(const std::string& path);
    template<typename T> bool WriteFile(std::string path, T data);
}

template<typename T>
bool Utils::WriteFile(std::string path, T data) {
    std::string str = Utils::ReadFileIntoString(path);

	std::ofstream out_file;
    out_file.open(path, std::ios::out | std::ios::trunc);
	if (!out_file.is_open()) {
		return false;
	}

    while (str.size() > MAX_RATE_RECORD) { //大于MAX_SIZE清零后重新写入
        str.erase(0, str.find_first_of("\n") + 1);
    }

    out_file << str;
	out_file << Utils::getCurrentTime0() << "\t" << data << std::endl;
	out_file.close();
	return true;
}