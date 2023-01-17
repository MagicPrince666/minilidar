#pragma once

#include "xepoll.h"
#include "interface.h"

class Ultrasonic
{
public:
    Ultrasonic(std::string dev = "srf04");
    ~Ultrasonic();

    int Srf04Distance();

private:
    std::string device_dir_;
    std::string ScanIioDevice(std::string name);
};
