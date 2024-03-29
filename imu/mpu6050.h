/**
 * @file mpu6050.h
 * @author 黄李全 (846863428@qq.com)
 * @brief 陀螺仪数据获取及解算
 * @version 0.1
 * @date 2023-02-02
 * @copyright Copyright (c) {2023} 个人版权所有,仅供学习
 */
#pragma once

#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include "gpio_key.h"
#include "driver_mpu6050_dmp.h"

typedef struct
{
    float w;
    float x;
    float y;
    float z;
} quaternion_t;

typedef struct
{
    float roll;
    float pitch;
    float yaw;
} euler_t;

class Mpu6050 {
public:
    Mpu6050(std::string dev = "mpu6050");
   virtual ~Mpu6050();

    virtual bool Init();
    int Mpu6050Test();
    bool GetEulerQuat(euler_t &eu);

private:
    std::string device_;
    GpioKey *mpu_int_;
    uint8_t (*gpio_irq_)(void) = nullptr;
    int16_t gs_accel_raw_[128][3];
    float gs_accel_g_[128][3];
    int16_t gs_gyro_raw_[128][3];
    float gs_gyro_dps_[128][3];
    int32_t gs_quat_[128][4];
    float gs_pitch_[128];
    float gs_roll_[128];
    float gs_yaw_[128];
    mpu6050_address_t addr_;

    void Euler2Quaternion(float roll, float pitch, float yaw, quaternion_t &quat);

protected:
    int GpioInterruptInit();

    void GpioInterruptDeinit();
};
