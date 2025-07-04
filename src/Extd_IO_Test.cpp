#include <Arduino.h>
#include <Wire.h>
#include "Extd_IO_Test.h"

//根据芯片原理图，A0=0, A1=0, A2=1;
#define PCA9534_ADDR 0x24
#define REG_INPUT 0x00 // 输入寄存器
#define REG_OUTPUT 0x01 // 输出寄存器   
#define REG_POLARITY 0x02 // 极性寄存器
#define REG_CONFIG 0x03 // 配置寄存器

// 引脚定义 (ESP32)
const int sdaPin = 41;
const int sclPin = 42;
void PCA_Write(uint8_t reg, uint8_t data);


void RTC_SCL_init() 
{
	Wire.begin(sdaPin, sclPin); // 初始化I2C，指定SDA和SCL引脚
    PCA_Write(REG_CONFIG, 0xF0); //设置P0~P4为输出，P5~P7为输入
}

//写入PCA9534寄存器
void PCA_Write(uint8_t reg, uint8_t data) {
    Wire.beginTransmission(PCA9534_ADDR); //写入PCA9534地址，开始I2C通信
    Wire.write(reg); // 写入寄存器地址
    Wire.write(data); // 写入数据
    Wire.endTransmission(); // 结束I2C通信
    if (Wire.endTransmission() != 0) {
        Serial.println("Error: Write failed");
    }    

}

// 读取PCA9534寄存器
uint8_t PCA_Read(uint8_t reg) {
  Wire.beginTransmission(PCA9534_ADDR);
  Wire.write(reg);
  Wire.endTransmission();
  Wire.requestFrom(PCA9534_ADDR, 1);
  return Wire.read();
}

// 二进制打印辅助函数, 因为在读取与写入测试中，
// 只需要关注低4位和高4位，所以只打印4位二进制
void printBinary(uint8_t value) {
  for (int8_t i = 3; i >= 0; i--) {
    Serial.write(value & (1 << i) ? '1' : '0');
  }
}


