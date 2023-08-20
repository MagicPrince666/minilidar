#pragma once

#include <vector>
#include "xepoll.h"
#include "interface.h"

class TouchScreen
{
public:
    TouchScreen();
    ~TouchScreen();

    bool Init();
    int ReadTouchData();

private:
    int key_input_fd_{-1};
};
