#include <Arduino.h>
#include <Wire.h>
#include "AHT20.h"
#include "AddrTest.h"
#include "Extd_IO_Test.h"
#include <driver/rtc_io.h>
#include "LoRa_Wakeup.h"
#include "LoRa_Handler.h"
#include "LoRa_Config.h"
#include "System_Operations.h"
#include "esp_camera.h"
#include <WiFi.h>
#include "CameraManager.h"
#include "camera_pins.h"
#include "ADC_test.h"
#include "WebServer.h"
#include "Web_LoRa_Server.h"

// Configure the camera
#define MY_CAM

// I2C总线配置
#define RTC_SDA_PIN 41
#define RTC_SCL_PIN 42

// I2C时钟速率配置 (单位: Hz)
#define RTC_I2C_CLOCK 100000 // 100kHz

#define AUX_PIN 3 // 唤醒引脚，必须使用esp32-GPIO

void verifyAllDevices();

// 使用RTC内存保存唤醒计数
RTC_DATA_ATTR int wakeCount = 0;

// ===========================
// Enter your WiFi credentials
// ===========================
const char *ssid = "rbx";
const char *password = "rbx@20230101";

void startCameraServer();

// 冷启动初始化
void cold_boot_init()
{
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

    // adc_init(2);

    Wire.begin(RTC_SDA_PIN, RTC_SCL_PIN); // Initialize I2C bus 1 (RTC_I2C)
    Wire.setClock(RTC_I2C_CLOCK);         // Set RTC I2C bus clock frequency
    Serial.printf("\nRTC I2C bus initialized (SDA:%d, SCL:%d)\n", RTC_SDA_PIN, RTC_SCL_PIN);
    Serial.printf("RTC I2C clock: %d Hz\n", RTC_I2C_CLOCK);

    // Wire.begin(39, 40);           // Initialize OV bus 1 (RTC_I2C)
    // Wire.setClock(RTC_I2C_CLOCK); // Set RTC I2C bus clock frequency
    // Serial.printf("RTC I2C bus initialized (SDA:%d, SCL:%d)\n", RTC_SDA_PIN, RTC_SCL_PIN);
    // Serial.printf("RTC I2C clock: %d Hz\n", RTC_I2C_CLOCK);

    // Initialize second I2C bus, NOT USE ANYMORE, BUT RESERVE THIS FUNCTION
    /*TwoWire secondI2C = TwoWire(1);  // 使用I2C端口1
    secondI2C.begin(SECOND_SDA_PIN, SECOND_SCL_PIN);
    secondI2C.setClock(SECOND_I2C_CLOCK); // Set second I2C bus clock frequency
    Serial.printf("Second I2C bus initialized (SDA:%d, SCL:%d)\n", SECOND_SDA_PIN, SECOND_SCL_PIN);
    Serial.printf("Second I2C clock: %d Hz\n", SECOND_I2C_CLOCK); */

    delay(500); // Wait for serial to stabilize

    // Verify all device addresses
    verifyAllDevices();

    // 增加唤醒计数
    wakeCount++;
    Serial.printf("\n[System] Boot #%d\n\n", wakeCount);

    // 先配置LoRa模块
    LoRa_Config_Init();

    // 初始化LoRa唤醒模块
    lora_wakeup_init(AUX_PIN);

    // // 启用定时器唤醒 (每5分钟唤醒一次检查状态)
    // lora_enable_timer_wakeup(5 * 60 * 1000000ULL); // 5分钟

    // 处理唤醒事件
    lora_handle_wakeup();

    // 如果是冷启动，执行特殊初始化
    if (is_cold_boot())
    {
        cold_boot_init();
        system_hardware_init();
        if (system_self_test() != SYS_STATUS_OK)
        {
            Serial.println("[System] Self test failed! Entering deep sleep...");
            delay(1000);
            // lora_enter_deep_sleep();
        }
    }
    else
    {
        // 非冷启动时也需要重新初始化LoRa通信
        lora_comm_init();
    }

    // webServer_init();

    // 根据唤醒原因执行不同操作
    switch (lora_get_wakeup_reason())
    {
    case WAKEUP_COLD_BOOT:
        Serial.println("[System] Cold boot detected");
        // 冷启动的特殊处理已在上面完成
        break;

    case WAKEUP_LORA_DATA:
        Serial.println("[System] LoRa data received wakeup");
        // 确保数据仍然可用
        if (lora_data_available())
        {
            process_lora_data();
            /* 此处加入唤醒后的操作 */
        }
        else
        {
            Serial.println("[LoRa] No data available after wakeup");
        }
        break;

    case WAKEUP_TIMER:
        Serial.println("[System] Periodic wakeup for status check");
        // 这里可以添加系统状态检查代码

        // 示例：发送状态报告
        {
            const char *statusMsg = "Status: OK";
            lora_send_data((const uint8_t *)statusMsg, strlen(statusMsg));
        }
        break;

    default:
        Serial.println("[System] Unknown wakeup reason");
        break;
    }

    // Camera Initialization and Configuration
    // CAM_init();

    // Wifi Initialization

    // Serial.println("\n== WiFi Initialization == ");

    // WiFi.begin(ssid, password);
    // WiFi.setSleep(false);

    // while (WiFi.status() != WL_CONNECTED)
    // {
    //     delay(500);
    //     Serial.print(".");
    // }
    // Serial.println("");
    // Serial.println("WiFi connected");

    // startCameraServer();

    // Serial.print("Camera Ready! Use 'http://");
    // Serial.print(WiFi.localIP());
    // Serial.println("' to connect");

    // 进入深度睡眠
    Serial.println("Returning to deep sleep...");
    delay(100); // 确保串口消息发送完成
    lora_enter_deep_sleep();
}

void loop()
{
    // Serial.println("\n\nRaw: ");
    // Serial.println(adc_read_raw());

    // Serial.println("Voltage: ");
    // Serial.println(adc_read_volts() / 1000.0f);

    // Serial.println("Voltage: ");
    // Serial.println((adc_read_volts() / 1000.0f) * 6.1);

    // Serial.println("Filtered: ");
    // Serial.println(adc_read_filtered(8));

    // delay(1000);
}
