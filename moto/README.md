## 舵机驱动
舵机的控制由pwm来控制，控制周期T=20ms 高电平范围1-2ms,对应角度0-90度
```
echo 0 > /sys/class/pwm/pwmchip0/export
echo 20000000 > /sys/class/pwm/pwmchip0/pwm0/period
echo 1500000 > /sys/class/pwm/pwmchip0/pwm0/duty_cycle
echo normal > /sys/class/pwm/pwmchip0/pwm0/polarity
echo 1 > /sys/class/pwm/pwmchip0/pwm0/enable
```