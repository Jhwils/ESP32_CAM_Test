#ifndef LORA_WAKEUP_H
#define LORA_WAKEUP_H

#include <Arduino.h>
#include <driver/rtc_io.h>
#include <esp_sleep.h>

#ifdef __cplusplus
extern "C" {
#endif

// 定义唤醒原因枚举
typedef enum {
    WAKEUP_UNKNOWN = 0,
    WAKEUP_COLD_BOOT,      // 冷启动（首次上电）
    WAKEUP_LORA_DATA,
    WAKEUP_TIMER,
    WAKEUP_BUTTON,
    WAKEUP_OTHER
} wakeup_reason_t;

// LoRa唤醒模块初始化
void lora_wakeup_init(uint8_t aux_pin);

// 进入深度睡眠
void lora_enter_deep_sleep();

// 获取唤醒原因
wakeup_reason_t lora_get_wakeup_reason();

// 获取唤醒原因字符串
const char* lora_wakeup_reason_string(wakeup_reason_t reason);

// 处理唤醒事件
void lora_handle_wakeup();

// 检查是否有LoRa数据
bool lora_data_available();

// 启用定时器唤醒
void lora_enable_timer_wakeup(uint64_t time_in_us);

// 启用按钮唤醒
void lora_enable_button_wakeup(uint8_t button_pin);

// 检查是否是冷启动
bool is_cold_boot();

#ifdef __cplusplus
}
#endif

#endif // LORA_WAKEUP_H