#include<Arduino.h>
#include<driver/rtc_io.h>
#include<esp_sleep.h>
#include"LoRa_Wakeup.h"

// 在RTC内存中保存冷启动标志
RTC_DATA_ATTR static bool cold_boot = true;

// LoRa唤醒模块结构体
typedef struct {
    uint8_t aux_pin;          // LoRa AUX引脚
    wakeup_reason_t reason;   // 唤醒原因
    bool is_initialized;      // 初始化标志
} lora_wakeup_t;

// 全局实例
static lora_wakeup_t lora_wakeup = {
    .aux_pin = 0,
    .reason = WAKEUP_UNKNOWN,
    .is_initialized = false
};

// 初始化LoRa唤醒模块
void lora_wakeup_init(uint8_t aux_pin) {
    if (lora_wakeup.is_initialized) return;
    
    // 保存配置
    lora_wakeup.aux_pin = aux_pin;
    
    // 配置AUX引脚为输入，启用上拉电阻
    pinMode(lora_wakeup.aux_pin, INPUT_PULLUP);
    
    // 配置RTC GPIO作为唤醒源
    esp_sleep_enable_ext0_wakeup((gpio_num_t)lora_wakeup.aux_pin, 0); // 低电平触发
    
    // 设置初始化标志
    lora_wakeup.is_initialized = true;
    
    Serial.println("\n[LoRa Wakeup] Module initialized");
}

// 进入深度睡眠
void lora_enter_deep_sleep() {
    if (!lora_wakeup.is_initialized) {
        Serial.println("[LoRa Wakeup] Error: Module not initialized!");
        return;
    }

    // 标记冷启动已完成
    cold_boot = false;
    
    Serial.println("[LoRa Wakeup] Entering deep sleep...\n\n");
    Serial.println("== End of this part ==\n");
    Serial.flush();
    
    // 断开所有不需要的外设以降低功耗，写在此处


    // 这里可以添加更多低功耗设置
    
    // 进入深度睡眠
    esp_deep_sleep_start();
}

// 获取唤醒原因
wakeup_reason_t lora_get_wakeup_reason() {
    return lora_wakeup.reason;
}

// 获取唤醒原因字符串
const char* lora_wakeup_reason_string(wakeup_reason_t reason) {
    switch(reason) {
        case WAKEUP_COLD_BOOT: return "Cold Boot";
        case WAKEUP_LORA_DATA: return "LoRa Data Received";
        case WAKEUP_TIMER: return "Timer";
        case WAKEUP_OTHER: return "Other";
        default: return "Unknown";
    }
}

// 处理唤醒事件
void lora_handle_wakeup() {
    // 如果是冷启动
    if (cold_boot) {
        Serial.println("[Wakeup] Cold boot (first initialization)");
        lora_wakeup.reason = WAKEUP_COLD_BOOT;
    } 
    // 否则是深度睡眠唤醒
    else {
        // 获取ESP32唤醒原因
        esp_sleep_wakeup_cause_t esp_reason = esp_sleep_get_wakeup_cause();
        
        switch(esp_reason) {
            case ESP_SLEEP_WAKEUP_EXT0:
                Serial.println("[Wakeup] External signal (RTC_IO) - LoRa data");
                lora_wakeup.reason = WAKEUP_LORA_DATA;
                break;
                
            case ESP_SLEEP_WAKEUP_EXT1: {
            // 获取EXT1唤醒状态
            uint64_t wakeup_pin_mask = esp_sleep_get_ext1_wakeup_status();

                Serial.println("[Wakeup] External signal (RTC_CNTL) from other pin");
                lora_wakeup.reason = WAKEUP_OTHER;
            
            break;
        }
                
            case ESP_SLEEP_WAKEUP_TIMER:
                Serial.println("[Wakeup] Timer");
                lora_wakeup.reason = WAKEUP_TIMER;
                break;
                
            case ESP_SLEEP_WAKEUP_TOUCHPAD:
                Serial.println("[Wakeup] Touchpad");
                lora_wakeup.reason = WAKEUP_OTHER;
                break;
                
            case ESP_SLEEP_WAKEUP_ULP:
                Serial.println("[Wakeup] ULP program");
                lora_wakeup.reason = WAKEUP_OTHER;
                break;
                
            default:
                Serial.printf("[Wakeup] Unknown reason: %d\n", esp_reason);
                lora_wakeup.reason = WAKEUP_UNKNOWN;
                break;
        }
    }
    
    Serial.print("Wakeup reason: ");
    Serial.println(lora_wakeup_reason_string(lora_wakeup.reason));
}

// 检查是否有LoRa数据
bool lora_data_available() {
    if (!lora_wakeup.is_initialized) return false;
    
    // 检查AUX引脚状态（低电平表示有数据）
    return digitalRead(lora_wakeup.aux_pin) == LOW;
}

// 启用定时器唤醒
void lora_enable_timer_wakeup(uint64_t time_in_us) {
    esp_sleep_enable_timer_wakeup(time_in_us);
    Serial.printf("[LoRa Wakeup] Timer wakeup enabled: %llu us\n", time_in_us);
}

// 检查是否是冷启动
bool is_cold_boot() {
    return cold_boot;
}