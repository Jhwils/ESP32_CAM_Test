#include "CameraManager.h"

bool CAM_init()
{
    powerOnCamera();

    camera_config_t config;
    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer = LEDC_TIMER_0;

    config.pin_d0 = Y2_GPIO_NUM;
    config.pin_d1 = Y3_GPIO_NUM;
    config.pin_d2 = Y4_GPIO_NUM;
    config.pin_d3 = Y5_GPIO_NUM;
    config.pin_d4 = Y6_GPIO_NUM;
    config.pin_d5 = Y7_GPIO_NUM;
    config.pin_d6 = Y8_GPIO_NUM;
    config.pin_d7 = Y9_GPIO_NUM;

    config.pin_xclk = XCLK_GPIO_NUM;
    config.pin_pclk = PCLK_GPIO_NUM;
    config.pin_vsync = VSYNC_GPIO_NUM;
    config.pin_href = HREF_GPIO_NUM;

    config.pin_sccb_sda = SIOD_GPIO_NUM;
    config.pin_sccb_scl = SIOC_GPIO_NUM;

    config.pin_pwdn = PWDN_GPIO_NUM;
    config.pin_reset = RESET_GPIO_NUM;

    config.xclk_freq_hz = 20000000;
    config.frame_size = FRAMESIZE_UXGA;
    config.pixel_format = PIXFORMAT_JPEG; // for streaming
    // config.pixel_format = PIXFORMAT_RGB565; // for face detection/recognition
    config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
    config.fb_location = CAMERA_FB_IN_PSRAM;
    config.jpeg_quality = 12;
    config.fb_count = 1;

    // if PSRAM IC present, init with UXGA resolution and higher JPEG quality
    //                      for larger pre-allocated frame buffer.
    if (config.pixel_format == PIXFORMAT_JPEG)
    {
        if (psramFound())
        {
            Serial.println("[Camera] PSRAM detected, using optimized configuration");
            config.jpeg_quality = 10;
            config.fb_count = 2;
            config.grab_mode = CAMERA_GRAB_LATEST;
        }
        else
        {
            Serial.println("[Camera] PSRAM not detected, using basic configuration");
            // Limit the frame size when PSRAM is not available
            config.frame_size = FRAMESIZE_SVGA;
            config.fb_location = CAMERA_FB_IN_DRAM;
        }
    }
    else
    {
        // Best option for face detection/recognition
        config.frame_size = FRAMESIZE_240X240;
#if CONFIG_IDF_TARGET_ESP32S3
        config.fb_count = 2;
#endif
    }

#if defined(CAMERA_MODEL_ESP_EYE)
    pinMode(13, INPUT_PULLUP);
    pinMode(14, INPUT_PULLUP);
#endif

    // camera init
    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK)
    {
        Serial.printf("[Camera] Camera initialization failed: 0x%x\n", err);
        powerOffCamera();
        return false;
    }

    sensor_t *s = esp_camera_sensor_get();
    // initial sensors are flipped vertically and colors are a bit saturated
    if (s->id.PID == OV3660_PID)
    {
        s->set_vflip(s, 1);       // flip it back
        s->set_brightness(s, 1);  // up the brightness just a bit
        s->set_saturation(s, -2); // lower the saturation
    }

    // **新增**：为OV5640相机添加特殊配置
    if (s->id.PID == OV5640_PID)
    {
        Serial.println("[Camera] OV5640 camera detected, applying specific configuration");
        // ESP32S3_EYE硬件需要垂直翻转
        s->set_vflip(s, 1); // 垂直翻转
        // OV5640在某些情况下可能需要水平镜像
        s->set_hmirror(s, 0); // 根据实际情况调整
        // 优化OV5640的色彩和亮度
        s->set_brightness(s, 0); // 标准亮度
        s->set_saturation(s, 0); // 标准饱和度
        s->set_contrast(s, 0);   // 标准对比度
    }
    else
    {
        Serial.printf("[Camera] Camera PID detected: 0x%04X\n", s->id.PID);
    }

    // 通用参数设置
    s->set_special_effect(s, 0);             // 0 = 无特效
    s->set_whitebal(s, 1);                   // 0 = 禁用, 1 = 启用
    s->set_awb_gain(s, 1);                   // 0 = 禁用, 1 = 启用
    s->set_wb_mode(s, 0);                    // 0 自动, 1 阳光, 2 多云, 3 办公室, 4 家庭
    s->set_exposure_ctrl(s, 1);              // 0 = 禁用, 1 = 启用
    s->set_aec2(s, 1);                       // 0 = 禁用, 1 = 启用
    s->set_ae_level(s, 0);                   // -2 to 2
    s->set_aec_value(s, 300);                // 0 to 1200
    s->set_gain_ctrl(s, 1);                  // 0 = 禁用, 1 = 启用
    s->set_agc_gain(s, 0);                   // 0 to 30
    s->set_gainceiling(s, (gainceiling_t)0); // 0 to 6
    s->set_bpc(s, 0);                        // 0 = 禁用, 1 = 启用
    s->set_wpc(s, 1);                        // 0 = 禁用, 1 = 启用
    s->set_raw_gma(s, 1);                    // 0 = 禁用, 1 = 启用
    s->set_lenc(s, 1);                       // 0 = 禁用, 1 = 启用
    s->set_dcw(s, 1);                        // 0 = 禁用, 1 = 启用
    s->set_colorbar(s, 0);                   // 0 = 禁用, 1 = 启用

    // drop down frame size for higher initial frame rate
    if (config.pixel_format == PIXFORMAT_JPEG)
    {
        s->set_framesize(s, FRAMESIZE_QVGA);
        Serial.println("[Camera] Set stream mode resolution to HD (1280x720)");
    }

    Serial.println("[Camera] Camera initialization successful, using CameraWebServer optimized configuration");
    return true;
}

camera_fb_t *captureFrame()
{
    Serial.println("[Camera] Starting photo capture...");

    // 确保相机电源开启
    powerOnCamera();

    // 检查相机状态
    sensor_t *s = esp_camera_sensor_get();
    if (!s)
    {
        Serial.println("[Camera] Unable to get sensor, camera may not be properly initialized");
        return NULL;
    }
    else
    {
        Serial.println("[Camera] Camera properly initialized");
    }

    // 确保设置为拍照模式参数
    s->set_framesize(s, FRAMESIZE_UXGA); // 高分辨率拍照
    s->set_quality(s, 10);               // 高质量
}

void powerOnCamera()
{
    // 首先将GPIO13设置为高电平，打开相机电源
    digitalWrite(CAMERA_POWER_PIN, HIGH);
    Serial.println("[Camera] Camera power ON (GPIO13=HIGH)");
    delay(100); // 等待电源稳定

    // 首先释放GPIO保持状态（如果之前启用了的话）
    esp_err_t hold_result = gpio_hold_dis((gpio_num_t)OV_PWDN_PIN);
    if (hold_result == ESP_OK)
    {
        Serial.println("[Camera] GPIO hold state for OV_PWDN pin released");
    }
    else
    {
        Serial.printf("[Camera] Failed to release GPIO hold state: 0x%x\n", hold_result);
    }

    digitalWrite(OV_PWDN_PIN, LOW);
    Serial.println("[Camera] Camera power ON (OV_PWDN=LOW)");
    delay(500); // 给相机一点时间启动
}

void powerOffCamera()
{
    // **重要修改**：不再调用esp_camera_deinit()避免GDMA错误
    // CameraWebServer的成功模式是保持相机驱动运行状态
    // esp_camera_deinit();  // 注释掉这行

    // 只控制电源引脚，但保持驱动程序运行
    digitalWrite(OV_PWDN_PIN, HIGH);
    Serial.println("[Camera] Camera power OFF (OV_PWDN=HIGH), but keeping driver running to avoid GDMA error");

    // 启用GPIO保持功能，确保深度睡眠时引脚保持高电平
    esp_err_t hold_result = gpio_hold_en((gpio_num_t)OV_PWDN_PIN);
    if (hold_result == ESP_OK)
    {
        Serial.println("[Camera] GPIO hold state enabled for OV_PWDN pin, will maintain HIGH level during deep sleep");
    }
    else
    {
        Serial.printf("[Camera] Failed to enable GPIO hold state: 0x%x, pin state may be lost during deep sleep\n", hold_result);
    }

    // 最后将GPIO13设置为低电平，关闭相机电源
    digitalWrite(CAMERA_POWER_PIN, LOW);
    Serial.println("[Camera] Camera power OFF (GPIO13=LOW)");
}