#include <Arduino.h>
#include <Wire.h>
#include "C:\Repositories\i2c_Test\i2c_test\include\TestTemp.h"
#include "C:\Repositories\i2c_Test\i2c_test\include\AddrTest.h"


void setup() {
  Serial.begin(115200); // 初始化串口通信
  Wire.begin(10,11); // 初始化I2C，默认引脚(SDA=21, SCL=22)
  Serial.println("\nI2C Scanner Starting...");
  Addrtest(); // 扫描I2C设备地址

  aht20_init();  // 初始化传感器
  delay(100);    // 稳定时间
}

void loop() {
  temptest();    // 执行温湿度测试
  delay(2000);   // 每2秒读取一次
}





