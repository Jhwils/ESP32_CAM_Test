#include "System_Operations.h"

// 初始化系统硬件
void system_hardware_init()
{
    // 初始化摄像机控制引脚
    pinMode(PWR_SW_CAM, OUTPUT);
    pinMode(OV_PWDN, OUTPUT);

    // 初始状态
    digitalWrite(PWR_SW_CAM, LOW);
    digitalWrite(OV_PWDN, LOW);

    Serial.println("[System] Hardware initialized");
}

system_status_t system_self_test()
{

    Serial.println("\n===== SYSTEM SELF TEST =====");

    // 1. 检查I2C设备
    Serial.println("[Test] Checking I2C devices...");
    Wire.beginTransmission(PCA9534_ADDR);
    if (Wire.endTransmission() != 0)
    {
        Serial.println("[Error] PCA9534 not responding!");
        return SYS_STATUS_I2C_ERROR;
    }
    else
    {
        Serial.println("[System] PCA9534 was successfully scaned");
    }

    Wire.beginTransmission(AHT20_ADDR);
    if (Wire.endTransmission() != 0)
    {
        Serial.println("[Error] AHT20 not responding!");
        return SYS_STATUS_SENSOR_ERROR;
    }
    else
    {
        Serial.println("[System] AHT20 was successfully scaned");
    }

    // Motor test
    Serial.println("[Test] Testing motor...");
    /* Waited to insert acutal code */

    // Camera test
    Serial.println("[Test] Testing camera...");
    /* Waited to insert actual code */

    Serial.println("[Test] All tests passed!");
    return SYS_STATUS_OK;
}

// 电机控制函数
void motor_control(bool on, bool direc, TwoWire *wire)
{
    uint8_t currentOutput = PCA_Read(wire, REG_OUTPUT); // 读取当前输出状态

    if (on)
    {
        if (direc)
        {
            // 正转: IN1=HIGH, IN2=LOW
            currentOutput |= (1 << MOTOR_IN1_BIT);
            currentOutput &= ~(1 << MOTOR_IN2_BIT);
            Serial.println("[Motor] ON - Forward");
        }
        else
        {
            // 反转: IN1=LOW, IN2=HIGH
            currentOutput &= ~(1 << MOTOR_IN1_BIT);
            currentOutput |= (1 << MOTOR_IN2_BIT);
            Serial.println("[Motor] ON - Reverse");
        }
    }
    else
    {
        // 停止: IN1=LOW, IN2=LOW
        currentOutput &= ~(1 << MOTOR_IN1_BIT);
        currentOutput &= ~(1 << MOTOR_IN2_BIT);
        Serial.println("[Motor] OFF");
    }

    PCA_Write(wire, REG_OUTPUT, currentOutput);
}

// 获取系统状态描述
const char *get_system_status_string(system_status_t status)
{
    switch (status)
    {
    case SYS_STATUS_OK:
        return "All systems OK";
    case SYS_STATUS_I2C_ERROR:
        return "I2C communication error";
    case SYS_STATUS_SENSOR_ERROR:
        return "Sensor error";
    case SYS_STATUS_MOTOR_ERROR:
        return "Motor control error";
    case SYS_STATUS_CAMERA_ERROR:
        return "Camera error";
    default:
        return "Unknown status";
    }
}