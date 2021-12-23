#include <stdlib.h>
#include <unistd.h>
#include <strings.h>
#include <string.h>
#include <sys/types.h>
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <iostream>

#include "pwm.h"

Pwm::Pwm(int pwm) : pwm_pin_(pwm)
{
    setup_pwm(pwm); //pwm0
    pwm_polarity(false);
    pwm_period(20000000);
    pwm_duty_cycle(1500000);
    pwm_enable();
}

Pwm::~Pwm()
{
}

int Pwm::setup_pwm(int pin)
{
    FILE *set_export = NULL;
    sprintf(setpin_, "%d", pin);
    set_export = fopen("/sys/class/pwm/pwmchip0/export", "w");
    if (set_export == NULL)
    {
        printf("Can\'t open /sys/class/pwm/pwmchip0/export\n");
    }
    else
    {
        fprintf(set_export, setpin_);
    }
    fclose(set_export);
    return 0;
}

int Pwm::pwm_enable()
{
    FILE *set_enable = NULL;
    int len = sprintf(setpin_, "/sys/class/pwm/pwmchip0/pwm%d/enable", pwm_pin_);
    setpin_[len] = 0;
    set_enable = fopen(setpin_, "w");
    if (set_enable == NULL)
    {
        printf("open %s error\n", setpin_);
    }
    else {
        fprintf(set_enable, "1");
    }
    fclose(set_enable);
    return 0;
}

int Pwm::pwm_disable()
{
    FILE *set_enable = NULL;
    int len = sprintf(setpin_, "/sys/class/pwm/pwmchip0/pwm%d/enable", pwm_pin_);
    setpin_[len] = 0;
    set_enable = fopen(setpin_, "w");
    if (set_enable == NULL)
    {
        printf("open %s error\n", setpin_);
    }
    else {
        fprintf(set_enable, "0");
    }
    fclose(set_enable);
    return 0;
}

int Pwm::pwm_polarity(bool value)
{
    FILE *set_polarity = NULL;
    int len = sprintf(setpin_, "/sys/class/pwm/pwmchip0/pwm%d/polarity", pwm_pin_);
    setpin_[len] = 0;
    set_polarity = fopen(setpin_, "w");
    if (set_polarity == NULL)
    {
        printf("open %s error\n", setpin_);
    }
    else
    {
        if (!value)
        {
            fprintf(set_polarity, "normal");
        }
        else
        {
            fprintf(set_polarity, "inversed");
        }
    }
    fclose(set_polarity);
    return 0;
}

int Pwm::pwm_period(int value)
{
    FILE *set_period = NULL;
    int len = sprintf(setpin_, "/sys/class/pwm/pwmchip0/pwm%d/period", pwm_pin_);
    setpin_[len] = 0;
    set_period = fopen(setpin_, "w");
    if (set_period == NULL)
    {
        printf("open %s error\n", setpin_);
    }
    else
    {
        fprintf(set_period, "%d", value);
    }
    fclose(set_period);
    return 0;
}

int Pwm::pwm_duty_cycle(int value)
{
    FILE *set_duty_cycle = NULL;
    int len = sprintf(setpin_, "/sys/class/pwm/pwmchip0/pwm%d/duty_cycle", pwm_pin_);
    setpin_[len] = 0;
    set_duty_cycle = fopen(setpin_, "w");
    if (set_duty_cycle == NULL)
    {
        printf("open %s error\n", setpin_);
    }
    else
    {
        fprintf(set_duty_cycle, "%d", value);
    }
    fclose(set_duty_cycle);
    return 0;
}
