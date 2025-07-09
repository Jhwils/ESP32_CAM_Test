#include <Arduino.h>
#include <Wire.h>
#include "TestTemp.h"
#include "AddrTest.h"
#include "Extd_IO_Test.h"
#include <driver/rtc_io.h>
#include "LoRa_Wakeup.h"
#include"LoRa_Handler.h"

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

#define AUX_PIN 3 //唤醒引脚，必须使用esp32-GPIO

void verifyAllDevices();
//TwoWire secondI2C = TwoWire(1);  // 使用I2C端口1

// 使用RTC内存保存唤醒计数
RTC_DATA_ATTR int wakeCount = 0;

// 冷启动初始化
void cold_boot_init() {
    Serial.println("\n=== COLD BOOT INITIALIZATION ===");
    Serial.println("Performing one-time setup tasks...");
    
    // 初始化LoRa通信
    lora_comm_init();
    
    // 这里添加其他只应在冷启动时执行的代码
    // 例如：初始化外设、加载配置、校准传感器等
    
    delay(500); // 模拟初始化时间
    Serial.println("Cold boot initialization complete\n");
}

void setup() 
{
    Serial.begin(115200);
    delay(1000);

/*Wire.begin(RTC_SDA_PIN, RTC_SCL_PIN); // Initialize I2C bus 1 (RTC_I2C)
Wire.setClock(RTC_I2C_CLOCK); // Set RTC I2C bus clock frequency
Serial.printf("RTC I2C bus initialized (SDA:%d, SCL:%d)\n", RTC_SDA_PIN, RTC_SCL_PIN);
Serial.printf("RTC I2C clock: %d Hz\n", RTC_I2C_CLOCK);

// Initialize second I2C bus
/*secondI2C.begin(SECOND_SDA_PIN, SECOND_SCL_PIN);
secondI2C.setClock(SECOND_I2C_CLOCK); // Set second I2C bus clock frequency
Serial.printf("Second I2C bus initialized (SDA:%d, SCL:%d)\n", SECOND_SDA_PIN, SECOND_SCL_PIN);
Serial.printf("Second I2C clock: %d Hz\n", SECOND_I2C_CLOCK);

delay(500); // Wait for serial to stabilize

// Verify all device addresses
verifyAllDevices();
*/


// 增加唤醒计数
    wakeCount++;
    Serial.printf("\n\n[System] Boot #%d\n", wakeCount);

// 初始化LoRa唤醒模块
    lora_wakeup_init(AUX_PIN);

// 启用定时器唤醒 (每5分钟唤醒一次检查状态)
//  lora_enable_timer_wakeup(5 * 60 * 1000000ULL); // 5分钟

// 处理唤醒事件
    lora_handle_wakeup();

// 如果是冷启动，执行特殊初始化
    if (is_cold_boot()) {
        cold_boot_init();
    } else {
        // 非冷启动时也需要重新初始化LoRa通信
        lora_comm_init();
    }

// 根据唤醒原因执行不同操作
    switch(lora_get_wakeup_reason()) {
        case WAKEUP_COLD_BOOT:
            Serial.println("[System] Cold boot detected");
            // 冷启动的特殊处理已在上面完成
            break;
            
        case WAKEUP_LORA_DATA:
            Serial.println("[System] LoRa data received wakeup");
            // 确保数据仍然可用
            if (lora_data_available()) {
                process_lora_data();
            } else {
                Serial.println("[LoRa] No data available after wakeup");
            }
            break;
            
        case WAKEUP_TIMER:
            Serial.println("[System] Periodic wakeup for status check");
            // 这里可以添加系统状态检查代码
            
            // 示例：发送状态报告
            {
                const char* statusMsg = "Status: OK";
                lora_send_data((const uint8_t*)statusMsg, strlen(statusMsg));
            }
            break;
             
        default:
            Serial.println("[System] Unknown wakeup reason");
            break;
    }

// 进入深度睡眠
    Serial.println("Returning to deep sleep...");
    delay(100); // 确保串口消息发送完成
    lora_enter_deep_sleep();
}

void loop() {
  
}


// Verify all device addresses
void verifyAllDevices() {
  Serial.println("\n===== Start Address Verification =====");
  
  // Verify RTC bus device
  Serial.println("\n[RTC I2C Bus Device Verification]");
  Serial.printf("Target address: 0x%02X (PCA9534)\n", PCA9534_ADDR);
  Addrtest(&Wire, PCA9534_ADDR, "RTC Bus");
  
  // Verify second bus devices
  Serial.println("\n[RTC Bus Device Verification]");
  Serial.printf("Target addresses: 0x%02X and 0x%02X\n", AHT20_ADDR, W24C16_Addr);
  Addrtest(&Wire, AHT20_ADDR, "RTC Bus");
  Addrtest(&Wire, W24C16_Addr, "RTC Bus");
  
  Serial.println("\n===== Verification Complete =====\n");
}