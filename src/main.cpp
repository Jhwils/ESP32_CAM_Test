#include <Arduino.h>
#include <Wire.h>
#include "TestTemp.h"
#include "AddrTest.h"
#include "Extd_IO_Test.h"
#include <driver/rtc_io.h>
#include "LoRa_Test.h"

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

#define AUX_PIN 8 //唤醒引脚，必须使用esp32-GPIO

void verifyAllDevices();
TwoWire secondI2C = TwoWire(1);  // 使用I2C端口1

void setup() 
{
Serial.begin(115200);

Wire.begin(RTC_SDA_PIN, RTC_SCL_PIN); // Initialize I2C bus 1 (RTC_I2C)
Wire.setClock(RTC_I2C_CLOCK); // Set RTC I2C bus clock frequency
Serial.printf("RTC I2C bus initialized (SDA:%d, SCL:%d)\n", RTC_SDA_PIN, RTC_SCL_PIN);
Serial.printf("RTC I2C clock: %d Hz\n", RTC_I2C_CLOCK);

// Initialize second I2C bus
secondI2C.begin(SECOND_SDA_PIN, SECOND_SCL_PIN);
secondI2C.setClock(SECOND_I2C_CLOCK); // Set second I2C bus clock frequency
Serial.printf("Second I2C bus initialized (SDA:%d, SCL:%d)\n", SECOND_SDA_PIN, SECOND_SCL_PIN);
Serial.printf("Second I2C clock: %d Hz\n", SECOND_I2C_CLOCK);

delay(500); // Wait for serial to stabilize

// Verify all device addresses
verifyAllDevices();

// 配置AUX引脚
pinMode(AUX_PIN, INPUT);
rtc_gpio_hold_dis((gpio_num_t)AUX_PIN);


// 首次启动初始化
if(firstBoot) 
{
  LoRa_Init();
  firstBoot = false;
  Serial.println("System is successfully initialized.\n");
}

}

void loop() {
  // 1. 进入正常模式，检查是否有数据
  handleReceivedData(); // 可选，处理唤醒时收到的数据

  // 2. 进入WOR模式，准备再次低功耗
  setLoRaMode(MODE_WOR);

  // 3. 配置深度睡眠
  prepareSleep();

  Serial.println("Entering deep sleep...\n");
  delay(100); // 等待串口输出完成

  // 4. 进入深度睡眠，等待AUX唤醒
  esp_deep_sleep_start();
}


// Verify all device addresses
void verifyAllDevices() {
  Serial.println("\n===== Start Address Verification =====");
  
  // Verify RTC bus device
  Serial.println("\n[RTC I2C Bus Device Verification]");
  Serial.printf("Target address: 0x%02X (PCA9534)\n", PCA9534_ADDR);
  Addrtest(&Wire, PCA9534_ADDR, "RTC Bus");
  
  // Verify second bus devices
  Serial.println("\n[Second I2C Bus Device Verification]");
  Serial.printf("Target addresses: 0x%02X and 0x%02X\n", AHT20_ADDR, W24C16_Addr);
  Addrtest(&secondI2C, AHT20_ADDR, "Second Bus");
  Addrtest(&secondI2C, W24C16_Addr, "Second Bus");
  
  Serial.println("\n===== Verification Complete =====\n");
}




