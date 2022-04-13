#pragma once

#include <vector>
#include "xepoll.h"
#include "interface.h"

class Ultrasonic : public Interface
{
public:
    Ultrasonic();
    ~Ultrasonic();

    bool init();
    int IRKey();

    void Transfer(int num);

private:
    int key_input_fd_{-1};
    void getFiles(std::string path, std::vector<std::string>& files);
    struct timeval last_time_;
    int is_action_{0};
};
