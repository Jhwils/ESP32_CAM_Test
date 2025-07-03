#include <arduino.h>
#include <Wire.h>
#include "TestTemp.h"

#define AHT20_ADDR 0x38       // I2C地址
#define TRIG_MEAS  0xAC       // 触发测量命令
#define SOFT_RESET 0xBA       // 软件复位命令
#define INIT_CMD   0xBE       // 初始化命令
#define BUSY_BIT   0x80       // 忙状态位
#define STATUS_REG 0x71       // 状态寄存器读取地址

// AHT20初始化
bool aht20_init() {
  Wire.beginTransmission(AHT20_ADDR);
  Wire.write(INIT_CMD);
  Wire.write(0x08);  // 校准参数1
  Wire.write(0x00);  // 校准参数2
  if (Wire.endTransmission() != 0) return false;
  
  delay(50);  // 等待校准完成
  return true;
}

// 温湿度测量主函数
void temptest() {
  // === 1. 发送测量命令 ===
  Wire.beginTransmission(AHT20_ADDR);
  Wire.write(TRIG_MEAS);
  Wire.write(0x33);  // 测量参数
  Wire.write(0x00);  // 保留位
  if (Wire.endTransmission() != 0) {
    Serial.println("Error: Write failed");
    return;
  }
  
  // === 2. 等待80ms测量完成 ===
  delay(80);
  
  // === 3. 读取6字节数据 ===
  uint8_t data[6];
  Wire.requestFrom(AHT20_ADDR, (uint8_t)6); //发送0x71读取状态寄存器
  for (int i = 0; i < 6; i++) {
    data[i] = Wire.read();
  }
  
  // === 4. 校验状态寄存器 ===
  if (data[0] & BUSY_BIT) {
    Serial.println("Error: Sensor busy");
    return;
  }
  
  // === 5. 数据转换 ===
  uint32_t rawHumidity = ((uint32_t)data[1] << 12) |
                         ((uint32_t)data[2] << 4) |
                         ((uint32_t)data[3] >> 4);
                         
  uint32_t rawTemp = (((uint32_t)data[3] & 0x0F) << 16) |
                     ((uint32_t)data[4] << 8) |
                     data[5];
  
  // === 6. 计算实际值 ===
  float humidity = (rawHumidity * 100.0f) / (1 << 20);  // 2^20=1048576
  float temperature = (rawTemp * 200.0f) / (1 << 20) - 50.0f;
  
  // === 7. 结果输出 ===
  Serial.print("Temperature: ");
  Serial.print(temperature, 1);
  Serial.print("°C\tHumidity: ");
  Serial.print(humidity, 1);
  Serial.println("%");

}
