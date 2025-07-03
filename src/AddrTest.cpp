#include <Arduino.h>
#include <Wire.h>

void Addrtest(void)
{
  uint8_t error[2];
  uint8_t AHT20_Addr = 0x38;
  uint8_t W24C16_Addr = 0x50;
  uint8_t Addr[]= {AHT20_Addr, W24C16_Addr};
  int foundDevices = 0;

  Serial.println("-------------------");
  Serial.println("Scanning I2C bus...");
  
  for(uint8_t i=0;i<2;i++)
  {
    Wire.beginTransmission(Addr[i]);
    error[i] = Wire.endTransmission();
    
    if (error[0] == 0) 
    { 
      // AHT20设备响应成功
      Serial.print("AHT20 Device is found @ 0x38\n");
      foundDevices++;
    }

    else if(error[1] == 0)
    { 
      // W24C16设备响应成功
      Serial.print("W24C16 Device is found @ 0x50\n");
      foundDevices++;
    }

    error[i] = 1;
  }
  
  if (foundDevices == 0) 
  {
    Serial.println("No I2C devices found!");
  } 
  else 
  {
    Serial.println("Scan completed.");
  }
  
  Serial.println("-------------------\n");
  delay(5000); 
}