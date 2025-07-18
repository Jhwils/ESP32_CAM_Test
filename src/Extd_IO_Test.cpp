#include "Extd_IO_Test.h"

// 引脚定义 (ESP32)
const int sdaPin = 41;
const int sclPin = 42;

void RTC_SCL_init(void)
{
  Wire.begin(sdaPin, sclPin); // 初始化I2C，指定SDA和SCL引脚
}

// 写入PCA9534寄存器
bool PCA_Write(TwoWire *wire, uint8_t reg, uint8_t data)
{
  wire->beginTransmission(PCA9534_ADDR); // 写入PCA9534地址，开始I2C通信
  wire->write(reg);                      // 写入寄存器地址
  wire->write(data);                     // 写入数据
  if (wire->endTransmission() != 0)
  {
    Serial.println("Error: PCA9534 write failed");
    return false; // 写入失败，返回false
  }
  return true; // 写入成功，返回true
}

// 读取PCA9534寄存器
uint8_t PCA_Read(TwoWire *wire, uint8_t reg)
{                                        // 添加wire参数
  wire->beginTransmission(PCA9534_ADDR); // 开始传输到PCA9534_ADDR地址
  wire->write(reg);                      // 写入要读取的寄存器地址
  if (wire->endTransmission() != 0)
    return 0xFF; // 如果传输失败，返回0xFF

  wire->requestFrom(PCA9534_ADDR, 1);             // 从PCA9534_ADDR地址请求1个字节的数据
  return wire->available() ? wire->read() : 0xFF; // 如果有数据可读，返回读取的数据，否则返回0xFF
}

// 二进制打印辅助函数, 因为在读取与写入测试中，
// 只需要关注低4位和高4位，所以只打印4位二进制
void printBinary(uint8_t value)
{
  for (int8_t i = 3; i >= 0; i--)
  {
    Serial.write(value & (1 << i) ?'1':'0'); // value此时是右移4位的输入值，遍历这四位
  }
}
