#include <Arduino.h>
#include <Wire.h>
#include "TestTemp.h"
#include "AddrTest.h"
#include "Extd_IO_Test.h"

#define PCA9534_ADDR 0x24
#define REG_INPUT 0x00 // 输入寄存器
#define REG_OUTPUT 0x01 // 输出寄存器   
#define REG_POLARITY 0x02 // 极性寄存器
#define REG_CONFIG 0x03 // 配置寄存器

#define AHT20_ADDR 0x38 // AHT20传感器地址
#define W24C16_Addr 0x50 // W24C16 EEPROM地址

// I2C总线配置
#define RTC_SDA_PIN 41
#define RTC_SCL_PIN 42
#define SECOND_SDA_PIN 10  // 第二I2C总线SDA引脚
#define SECOND_SCL_PIN 11  // 第二I2C总线SCL引脚

// I2C时钟速率配置 (单位: Hz)
#define RTC_I2C_CLOCK 100000   // 100kHz
#define SECOND_I2C_CLOCK 100000 // 100kHzd

void verifyAllDevices();
TwoWire secondI2C = TwoWire(1);  // 使用I2C端口1

void setup() {
Serial.begin(115200);
Wire.begin(RTC_SDA_PIN, RTC_SCL_PIN); //初始化i2c总线1 RTC_I2C
Wire.setClock(RTC_I2C_CLOCK); // 设置RTC I2C总线时钟频率
Serial.printf("RTC I2C总线初始化完成 (SDA:%d, SCL:%d)\n", RTC_SDA_PIN, RTC_SCL_PIN);
Serial.printf("RTC I2C时钟频率: %d Hz\n", RTC_I2C_CLOCK);

// 初始化第二I2C总线
secondI2C.begin(SECOND_SDA_PIN, SECOND_SCL_PIN);
secondI2C.setClock(SECOND_I2C_CLOCK); // 设置第二I2C总线时钟频率
Serial.printf("第二I2C总线初始化完成 (SDA:%d, SCL:%d)\n", SECOND_SDA_PIN, SECOND_SCL_PIN);
Serial.printf("第二I2C时钟频率: %d Hz\n", SECOND_I2C_CLOCK);

while (!Serial);
Serial.println("\\nI2C Scanner");

delay(1000);

// 验证所有设备地址
verifyAllDevices();
}

void loop() {
  // 主循环不需要执行操作
  delay(10000);
  Serial.println("系统空闲...");
}



// 验证所有设备地址
void verifyAllDevices() {
  Serial.println("\n===== 开始地址验证 =====");
  
  // 验证RTC总线设备
  Serial.println("\n[验证RTC I2C总线设备]");
  Serial.printf("目标地址: 0x%02X (PCA9534)\n", PCA9534_ADDR);
  Addrtest(&Wire, PCA9534_ADDR, "RTC总线");
  
  // 验证第二总线设备
  Serial.println("\n[验证第二I2C总线设备]");
  Serial.printf("目标地址: 0x%02X 和 0x%02X\n", AHT20_ADDR, W24C16_Addr);
  Addrtest(&secondI2C, AHT20_ADDR, "第二总线");
  Addrtest(&secondI2C, W24C16_Addr, "第二总线");
  
  Serial.println("\n===== 验证完成 =====");
}




