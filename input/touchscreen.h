#pragma once

#include <vector>
#include "xepoll.h"
#include "interface.h"

class TouchScreen
{
public:
    TouchScreen(Interface *interface);
    ~TouchScreen();

    bool init();
    int ReadTouchData();

private:
    int key_input_fd_{-1};
    Interface *m_interface_;
};
