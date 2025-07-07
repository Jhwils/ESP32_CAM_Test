#include <Arduino.h>
#include <Wire.h>

#define PCA9534_ADDR 0x24

//I2C地址测试函数
void Addrtest(TwoWire *wire, uint8_t Addr, const char* busName) 
{
  Serial.println("-------------------");
  Serial.printf("Current I2C bus: %s, Scan Address: 0x%02X\n", busName, Addr);
  
  wire->beginTransmission(Addr);
  uint8_t error = wire->endTransmission();
  
  if (error == 0) { 
    Serial.printf("Device @ 0x%02X in %s was successfully scaned\n", Addr, busName);
  }
  else if (error == 1) {
    Serial.printf("Address 0x%02X in %s didn't response (Error Code 1 - Too long data)\n", Addr, busName);
  }
  else if (error == 2) {
    Serial.printf("Address 0x%02X in %s didn't response (Error Code 2 - Receive NACK)\n", Addr, busName);
  }
  else if (error == 3) {
    Serial.printf("Address 0x%02X in %s didn't response (Error Code 3 - Failed to transmit the data)\n", Addr, busName);
  }
  else {
    Serial.printf("Address 0x%02X in  %s received an unkown error (Error Code %d)\n", Addr, busName, error);
  }
  
  Serial.println("-------------------\n");
  delay(500); // 短延时避免总线拥塞
}