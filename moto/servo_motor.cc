#include "servo_motor.h"

ServoMotor::ServoMotor() {
    pwm_ = new Pwm(0);
    servo(45); //初始45度 正中
}

ServoMotor::~ServoMotor() {
    delete pwm_;
}

int ServoMotor::servo(int angle) { // 0 - 90度控制范围
    if (angle >= 0 && angle <= 90) {
        int duty = (double)angle/90.0*1000000 + 1000000;
        pwm_->pwm_duty_cycle(duty);
    }
    return 0;
}
