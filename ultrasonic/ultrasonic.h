#pragma once

#include <vector>
#include "xepoll.h"
#include "interface.h"

class GpioKey : public Interface
{
public:
    GpioKey();
    ~GpioKey();

    bool init();
    int IRKey();

    void Transfer(int num);

private:
    int key_input_fd_{-1};
    void getFiles(std::string path, std::vector<std::string>& files);
    struct timeval last_time_;
    int is_action_{0};
};
