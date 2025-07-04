#include <Arduino.h>
#include <Wire.h>
#include "C:\Repositories\i2c_Test\i2c_test\include\TestTemp.h"
#include "C:\Repositories\i2c_Test\i2c_test\include\AddrTest.h"
#include "Extd_IO_Test.h"

#define PCA9534_ADDR 0x24
#define REG_INPUT 0x00 // 输入寄存器
#define REG_OUTPUT 0x01 // 输出寄存器   
#define REG_POLARITY 0x02 // 极性寄存器
#define REG_CONFIG 0x03 // 配置寄存器



void setup() {
  Serial.begin(115200); // 初始化串口通信
  Wire.begin(10,11); // 初始化I2C，默认引脚(SDA=21, SCL=22)
  Serial.println("\nI2C Scanner Starting...");
  Addrtest(); // 扫描I2C设备地址

//  aht20_init();  // 初始化传感器
  delay(100);    // 稳定时间
}

void loop() {
/* temptest();    // 执行温湿度测试
  delay(2000);   // 每2秒读取一次*/ 

  // 测试1: LED流水灯 (使用低4位输出),即P0~P3
  static uint8_t ledPattern = 0x01;
  for (int i = 0; i < 4; i++) {
    PCA_Write(REG_OUTPUT, ledPattern);
    Serial.printf("LED状态: 0x%02X -> ", ledPattern);
    printBinary(ledPattern & 0x0F);  // 只显示低4位，屏蔽高四位
    
    // 测试2: 读取按钮状态 (高4位输入)，即P4~P7
    uint8_t buttons = PCA_Read(REG_INPUT) >> 4;  // 获取高4位
    Serial.print(" | 按钮状态: ");
    printBinary(buttons);
    
    //实现流水灯效果，从P0到P3循环点亮
    ledPattern = (ledPattern << 1) | (ledPattern >> 3);  // 循环左移
    delay(500);  // 效果可见
  }
  Serial.println("-------------------");

}





