#include <Arduino.h>
#include <Wire.h>

#define PCA9534_ADDR 0x24

//I2C地址测试函数
void Addrtest(TwoWire *wire, uint8_t Addr, const char* busName) 
{
  Serial.println("-------------------");
  Serial.printf("在 %s 上扫描地址: 0x%02X\n", busName, Addr);
  
  wire->beginTransmission(Addr);
  uint8_t error = wire->endTransmission();
  
  if (error == 0) { 
    Serial.printf("设备 @ 0x%02X 在 %s 上检测成功\n", Addr, busName);
  }
  else if (error == 1) {
    Serial.printf("地址 0x%02X 在 %s 上无响应 (错误代码: 1 - 数据过长)\n", Addr, busName);
  }
  else if (error == 2) {
    Serial.printf("地址 0x%02X 在 %s 上无响应 (错误代码: 2 - 收到NACK)\n", Addr, busName);
  }
  else if (error == 3) {
    Serial.printf("地址 0x%02X 在 %s 上无响应 (错误代码: 3 - 发送数据失败)\n", Addr, busName);
  }
  else {
    Serial.printf("地址 0x%02X 在 %s 上发生未知错误 (错误代码: %d)\n", Addr, busName, error);
  }
  
  Serial.println("-------------------\n");
  delay(500); // 短延时避免总线拥塞
}