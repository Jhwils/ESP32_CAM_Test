#include <Arduino.h>
#include <Wire.h>
#include "Extd_IO_Test.h"

//根据芯片原理图，A0=0, A1=0, A2=1;
#define PCA9534_ADDR 0x24

// 引脚定义 (ESP32)
const int sdaPin = 41;
const int sclPin = 42;



