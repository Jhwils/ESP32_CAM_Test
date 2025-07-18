#ifndef CAMERA_MANAGER_H
#define CAMERA_MANAGER_H

// 确保在包含相机头文件之前定义相机型号
#define BOARD_CAMERA_MODEL_ESP32_S3_EYE_new

#define MY_CAM

#include <Arduino.h>
#include <Wire.h>
#include "esp_camera.h"
#include"camera_pins.h"

// 定义OV5640相机的PWDN引脚
#define OV_PWDN_PIN 21

// 定义相机电源控制引脚
#define CAMERA_POWER_PIN 13

bool CAM_init();

camera_fb_t* captureFrame();


void powerOnCamera();

void powerOffCamera();

#endif // CAMERA_MANAGER_H