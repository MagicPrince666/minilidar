#include <new>
#include "servo_motor.h"

ServoMotor::ServoMotor() {
    PwmPram servo_pwm;
    pwm_ = new (std::nothrow)Pwm(servo_pwm);
    servo(45); //初始45度 正中
}

ServoMotor::~ServoMotor() {
    delete pwm_;
}

int ServoMotor::servo(int angle) { // 0 - 90度控制范围
    if (angle >= 0 && angle <= 90) {
        int duty = (double)angle/90.0*1000000 + 1000000;
        pwm_->PwmDutyCycle(duty);
    }
    return 0;
}
