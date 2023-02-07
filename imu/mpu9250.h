/**
 * @file mpu9250.h
 * @author 黄李全 (846863428@qq.com)
 * @brief
 * @version 0.1
 * @date 2023-02-08
 * @copyright Copyright (c) {2023} 个人版权所有,仅供学习
 */
#pragma once

#include "mpu6050.h"

class Mpu9250 : public Mpu6050
{
public:
    Mpu9250();
    ~Mpu9250();

    bool Init();

    int test();

private:
};