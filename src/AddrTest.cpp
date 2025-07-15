#include <Arduino.h>
#include <Wire.h>
#include"Extd_IO_Test.h"

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


// Verify all device addresses
void verifyAllDevices() {
  Serial.println("\n===== Start Address Verification =====");
  
  // Verify RTC bus device
  Serial.println("\n[RTC I2C Bus Device Verification]");
  Serial.printf("Target address: 0x%02X (PCA9534)\n", PCA9534_ADDR);
  Addrtest(&Wire, PCA9534_ADDR, "RTC Bus");
  
  // Verify second bus devices
  Serial.println("\n[RTC Bus Device Verification, through physical wire]");
  Serial.printf("Target addresses: 0x%02X and 0x%02X\n", AHT20_ADDR, W24C16_Addr);
  Addrtest(&Wire, AHT20_ADDR, "RTC Bus");
  Addrtest(&Wire, W24C16_Addr, "RTC Bus");
  
  Serial.println("\n===== Verification Complete =====\n");
}