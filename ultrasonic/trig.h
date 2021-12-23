#pragma once

#include <thread>
#include "interface.h"

class Trig
{
public:
    Trig(Interface *interface);
    ~Trig();

    int Action();

private:
    void TrigProcess();
    int trig_fd_{-1};
    std::thread trig_thread_;
    Interface *m_interface_;
};
