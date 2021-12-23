#pragma once

class Pwm
{
public:
    Pwm(int pwm = 0);
    ~Pwm();

    int setup_pwm(int pin);
    int pwm_enable();
    int pwm_disable();
    int pwm_polarity(bool value);
    int pwm_period(int value);
    int pwm_duty_cycle(int value);

private:
    char setpin_[64] = {0};
    int pwm_fd_{-1};
    int pwm_pin_{0};
};

