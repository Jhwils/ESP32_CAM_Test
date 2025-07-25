#ifndef LORA_HANDLER_H
#define LORA_HANDLER_H

#include <Arduino.h>
#include <HardwareSerial.h>

// LoRa通信配置
#define LORA_SERIAL Serial2 // 使用ESP32的Serial2
#define LORA_BAUDRATE 9600  // LoRa模块的波特率
#define LORA_RX_PIN 10      // LoRa TX -> ESP32 RX (GPIO10)
#define LORA_TX_PIN 11      // LoRa RX -> ESP32 TX (GPIO11)

// 初始化LoRa通信
void lora_comm_init();

// 处理接收到的LoRa数据
void process_lora_data();

// 检查是否有可用的LoRa数据
bool lora_has_data();

void lora_send_data(const uint8_t *data, size_t length);


// 检查LoRa数据可用性（别名函数）
bool lora_data_available();

// 处理Serial0输入并通过LoRa发送
void handle_serial_input();

// 发送字符串数据
void lora_send_string(const String &message);

#endif // LORA_HANDLER_H