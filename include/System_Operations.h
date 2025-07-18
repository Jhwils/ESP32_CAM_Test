#ifndef __SYSTEM_OPERATIONS_H
#define __SYSTEM_OPERATIONS_H

#include <Arduino.h>
#include <Wire.h>
#include "Extd_IO_Test.h"
#include "AddrTest.h"

#define PWR_SW_CAM 13 // 待修改，实际引脚
#define OV_PWDN 21

// 系统状态枚举
typedef enum
{
    SYS_STATUS_OK = 0,
    SYS_STATUS_I2C_ERROR,
    SYS_STATUS_SENSOR_ERROR,
    SYS_STATUS_MOTOR_ERROR,
    SYS_STATUS_CAMERA_ERROR
} system_status_t;

void system_hardware_init();
system_status_t system_self_test();
void motor_control(bool on, bool direc, TwoWire *wire);
const char *get_system_status_string(system_status_t status);

#endif
