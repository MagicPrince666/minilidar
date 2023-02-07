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
    Vl53l0x(std::string device = "vl53l0x");
    ~Vl53l0x();
    int GetDistance();

private:
    FILE* vl53l0x_fd_{nullptr};
    std::string device_;
    std::string ReadFileIntoString(const std::string& path);
};
