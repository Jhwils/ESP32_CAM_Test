#include "LoRa_Handler.h"

// 初始化LoRa通信
void lora_comm_init() {
    // 配置LoRa串口
    LORA_SERIAL.begin(LORA_BAUDRATE, SERIAL_8N1, LORA_RX_PIN, LORA_TX_PIN);
    Serial.println("[LoRa] Serial communication initialized");
    Serial.printf("  RX Pin: %d, TX Pin: %d, Baudrate: %d\n", 
                 LORA_RX_PIN, LORA_TX_PIN, LORA_BAUDRATE);
}

// 处理接收到的LoRa数据
void process_lora_data() {
    Serial.println("[LoRa] Processing received data...");
    
    // 检查是否有数据可用
    if (!lora_has_data()) {
        Serial.println("[LoRa] No data available in buffer");
        return;
    }
    
    // 读取所有可用数据
    Serial.println("Received LoRa data:");
    Serial.println("-------------------");
    
    int available = LORA_SERIAL.available();
    uint8_t buffer[256];
    size_t bytesRead = 0;
    
    // 读取数据，但不超过缓冲区大小
    while (LORA_SERIAL.available() > 0 && bytesRead < sizeof(buffer)) {
        buffer[bytesRead] = LORA_SERIAL.read();
        bytesRead++;
    }
    
    // 打印十六进制格式
    Serial.println("Hex format:");
    for (size_t i = 0; i < bytesRead; i++) {
        if (buffer[i] < 0x10) Serial.print("0");
        Serial.print(buffer[i], HEX);
        Serial.print(" ");
        if ((i + 1) % 16 == 0) Serial.println();
    }
    Serial.println();
    
    // 打印ASCII格式
    Serial.println("ASCII format:");
    for (size_t i = 0; i < bytesRead; i++) {
        if (buffer[i] >= 32 && buffer[i] <= 126) {
            Serial.write(buffer[i]);
        } else {
            Serial.print(".");
        }
        if ((i + 1) % 16 == 0) Serial.println();
    }
    Serial.println();
    
    Serial.println("-------------------");
    Serial.printf("[LoRa] Processed %d bytes of data\n", bytesRead);
}

// 检查是否有可用的LoRa数据
bool lora_has_data() {
    return LORA_SERIAL.available() > 0;
}

// 发送LoRa数据
void lora_send_data(const uint8_t* data, size_t length) {
    if (length == 0) return;
    
    Serial.printf("[LoRa] Sending %d bytes of data\n", length);
    
    // 发送数据
    LORA_SERIAL.write(data, length);
    
    // 确保数据发送完成
    LORA_SERIAL.flush();
    
    Serial.println("[LoRa] Data sent successfully");
}