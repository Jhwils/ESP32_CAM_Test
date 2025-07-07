#ifndef __LORA_TEST_H__
#define __LORA_TEST_H__

#include <Arduino.h>

// 枚举类型完整定义放在头文件
enum LoRaMode {
  MODE_NORMAL = 0,    // M0=0, M1=0
  MODE_WOR = 1,       // M0=1, M1=0
  MODE_CONFIG = 2,    // M0=0, M1=1
  MODE_SLEEP = 3      // M0=1, M1=1
};

void setLoRaMode(LoRaMode mode); 
void PCA_init(void);
void LoRa_Init();
void handleReceivedData();
void prepareSleep();

extern HardwareSerial loraSerial;
extern RTC_DATA_ATTR bool firstBoot;

#endif
