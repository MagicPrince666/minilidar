#pragma once

#include "pwm.h"

class ServoMotor
{
public:
    ServoMotor();
    ~ServoMotor();

    int servo(int angle);
private:
    Pwm *pwm_;
};
