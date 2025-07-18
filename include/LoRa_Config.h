#ifndef __LORA_CONFIG_H__
#define __LORA_CONFIG_H__

// 根据芯片原理图，A0=0, A1=0, A2=1;
#define PCA9534_ADDR 0x24
#define REG_INPUT 0x00    // 输入寄存器
#define REG_OUTPUT 0x01   // 输出寄存器
#define REG_POLARITY 0x02 // 极性寄存器
#define REG_CONFIG 0x03   // 配置寄存器

// I2C总线配置
#define RTC_SDA_PIN 41
#define RTC_SCL_PIN 42

#include <Arduino.h>

// 枚举类型完整定义放在头文件
enum LoRaMode
{
  MODE_NORMAL = 0, // M0=0, M1=0
  MODE_WOR = 1,    // M0=1, M1=0
  MODE_CONFIG = 2, // M0=0, M1=1
  MODE_SLEEP = 3   // M0=1, M1=1
};

void PCA_init(void);
void setLoRaMode(LoRaMode mode);
void LoRa_Config_Init();

#endif
