#include<Arduino.h>
#include <Wire.h>
#include <EByte_LoRa_E22_library.h>
#include "LoRa_Test.h"
#include <driver/rtc_io.h>

#define LoRa_Pin_M0 16
#define LoRa_Pin_M1 15
#define AUX_Pin 34

// 串口配置
HardwareSerial loraSerial(2); // 使用ESP32的第二个硬件串口
RTC_DATA_ATTR bool firstBoot = true;

// 工作模式设置
enum LoRaMode {
  MODE_NORMAL = 0,    // M0=0, M1=0
  MODE_WOR = 1,       // M0=1, M1=0
  MODE_CONFIG = 2,    // M0=0, M1=1
  MODE_SLEEP = 3      // M0=1, M1=1
};

//初始化loRa模块
void LoRa_Init()
{
    // 初始化LoRa模块的代码
    Serial.println("LoRa module initialized.");
    pinMode(LoRa_Pin_M0, OUTPUT);
    pinMode(LoRa_Pin_M1, OUTPUT);

}

