#include<Arduino.h>
#include <Wire.h>
#include "LoRa_Test.h"
#include <driver/rtc_io.h>
#include "Extd_IO_Test.h"
#include  <HardwareSerial.h>

#define LoRa_Pin_M0 16 //等待实际修改 
#define LoRa_Pin_M1 15
#define AUX_PIN 8 //唤醒引脚，必须使用esp32-GPIO

//根据芯片原理图，A0=0, A1=0, A2=1;
#define PCA9534_ADDR 0x24
#define REG_INPUT 0x00 // 输入寄存器
#define REG_OUTPUT 0x01 // 输出寄存器   
#define REG_POLARITY 0x02 // 极性寄存器
#define REG_CONFIG 0x03 // 配置寄存器

// I2C总线配置
#define RTC_SDA_PIN 41
#define RTC_SCL_PIN 42

#define LORA_M0_BIT 6        // P6对应M0
#define LORA_M1_BIT 7        // P7对应M1


// 串口配置
HardwareSerial loraSerial(2); // 使用ESP32的第二个硬件串口
RTC_DATA_ATTR bool firstBoot = true;

void PCA_init(void)
{
  Wire.begin(RTC_SDA_PIN, RTC_SCL_PIN); //初始化I2C

  // 配置P6和P7为输出 (其他引脚可设为输入)
  uint8_t config = 0xFF & ~(1 << LORA_M0_BIT) & ~(1 << LORA_M1_BIT);
  
  Wire.beginTransmission(PCA9534_ADDR);
  Wire.write(REG_CONFIG); // 配置寄存器
  Wire.write(config);     // P6和P7设为输出
  Wire.endTransmission();
  
  // 初始设置为WOR模式
  setLoRaMode(MODE_WOR);
}

// 设置LoRa模块工作模式 (通过PCA9534)
void setLoRaMode(LoRaMode mode) 
{
  uint8_t outputState = 0;
  
  // 根据模式设置M0和M1状态
  switch(mode) {
    case MODE_NORMAL: // M0=0, M1=0
      outputState &= ~(1 << LORA_M0_BIT);
      outputState &= ~(1 << LORA_M1_BIT);
      break;
    case MODE_WOR: // M0=1, M1=0
      outputState |= (1 << LORA_M0_BIT);
      outputState &= ~(1 << LORA_M1_BIT);
      break;
    case MODE_CONFIG: // M0=0, M1=1
      outputState &= ~(1 << LORA_M0_BIT);
      outputState |= (1 << LORA_M1_BIT);
      break;
    case MODE_SLEEP: // M0=1, M1=1
      outputState |= (1 << LORA_M0_BIT);
      outputState |= (1 << LORA_M1_BIT);
      break;
  }
  
  // 写入输出寄存器
  PCA_Write(REG_OUTPUT, outputState);
  
  delay(50); // 等待模式切换完成
}

void setWORPeriod(uint8_t periodHex) {
  // 进入配置模式
  setLoRaMode(MODE_CONFIG);
  delay(50);
  
  // 发送配置命令 (仅修改WOR周期)
  const uint8_t configCmd[] = {
    0xC0, 0x00, 0x09, // 头部
    0x00, 0x00,       // 地址
    0x62, 0x17, 0x23, // 波特率、速率、频道
    periodHex,         // WOR周期
    0x00, 0x00         // 选项、密钥
  };
  
  loraSerial.write(configCmd, sizeof(configCmd));
  delay(100);
  
  // 返回WOR模式
  setLoRaMode(MODE_WOR);
}


//初始化loRa模块
void LoRa_Init()
{
  // 初始化LoRa模块的代码

  // 初始化PCA9534
  PCA_init(); 

  
  // 进入配置模式
  setLoRaMode(MODE_CONFIG);

  // 配置串口
  loraSerial.begin(9600, SERIAL_8N1, 16, 17); // RX=16, TX=17， 测试中，相机并不占用

  // 发送配置命令 (115200bps, 频道23, WOR周期2000ms)
  const uint8_t configCmd[] = {
    0xC0, 0x00, 0x09, // 头部
    0x00,             // 地址高字节
    0x00,             // 地址低字节
    0x62,             // 波特率: 0x62=115200bps
    0x17,             // 空中速率: 0x17=2.4kbps
    0x23,             // 频道: 0x23=433MHz频段频道35
    0x44,             // 选项: WOR周期2000ms
    0x00,             // 选项: 透明传输
    0x00              // 加密密钥
  };
  
  loraSerial.write(configCmd, sizeof(configCmd));
  delay(100);

  // 切换回WOR模式
  setLoRaMode(MODE_WOR);
  Serial.println("LoRa Module has entered WOR mode.\n");
  
  Serial.println("LoRa module initialized.");
}

// 处理接收数据
void handleReceivedData() {
  // 切换到正常模式接收数据
  setLoRaMode(MODE_NORMAL);
  
  // 等待模块准备好
  while(digitalRead(AUX_PIN) == LOW) {
    delay(10);
  }
  
  // 读取可用数据
  if(loraSerial.available()) {
    Serial.print("收到数据: ");
    while(loraSerial.available()) {
      Serial.print((char)loraSerial.read());
    }
    Serial.println();
  }
  
  // 返回WOR模式
  setLoRaMode(MODE_WOR);
}

// 准备深度睡眠
void  prepareSleep() {
  // 配置AUX唤醒 (上升沿触发)
  esp_sleep_enable_ext0_wakeup((gpio_num_t)AUX_PIN, HIGH);
  
  // 保持AUX引脚配置
  rtc_gpio_hold_en((gpio_num_t)AUX_PIN);
  
  // 设置I2C引脚为输入模式
  pinMode(RTC_SDA_PIN, INPUT);
  pinMode(RTC_SCL_PIN, INPUT);
  
  // 断开所有非必要GPIO
  for(int i = 0; i < 40; i++) {
    if(i != AUX_PIN) {
      pinMode(i, INPUT);
    }
  }
}
