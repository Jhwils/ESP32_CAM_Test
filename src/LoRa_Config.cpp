#include"LoRa_Config.h"
#include<Arduino.h>
#include"LoRa_Handler.h"
#include<Wire.h>
#include"Extd_IO_Test.h"


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
  PCA_Write(&Wire,REG_OUTPUT, outputState);
  
  delay(50); // 等待模式切换完成
}

void LoRa_Config_Init() {
  Serial.println("[LoRa] Initializing LoRa configuration...");
  
  // 初始化PCA9534
  PCA_init();
  
  // 设置LoRa模块为配置模式
  setLoRaMode(MODE_CONFIG);

  //配置串口
  LORA_SERIAL.begin(LORA_BAUDRATE, SERIAL_8N1, LORA_RX_PIN, LORA_TX_PIN);

  // 配置命令 (9600bps, 频道23, WOR周期2000ms)
  const uint8_t configCmd[] = {
    0xC0, 0x00, 0x09, // 头部
    0x00,             // 地址高字节
    0x00,             // 地址低字节
    0x00,             // 网络地址，用于区分网络；相互通信时，应当设置为相同
    0x64,             // UART波特率及空中速率: 0x64= baud rate 9600bps, air velocity 2,4kbps
    0x20,             // 分包设定，环境噪声使能，发射功率，0x20 = 240字节分包， RSSI 环境噪声使能启用, 发射功率22dBm
    0x17,             // 信道控制 CH, 0x17 = 23.
    0x83,             // 启用RSSI字节，传输方式，中继功能，LBT使能，WOR模式，WOR周期。选项: RSSI启用，透传，WOR周期2000ms
    0x00,             // 密匙高字节
    0x00              // 密匙低字节
  };

  //发送配置命令
  LORA_SERIAL.write(configCmd, sizeof(configCmd));
  Serial.println("[LoRa] Configuration command sent");
  delay(100); // 等待配置完成

  //切换回WOR模式
  setLoRaMode(MODE_WOR);
  Serial.println("LoRa Module has entered WOR mode.\n");
  delay(100);
  
  Serial.println("[LoRa] Configuration initialized");
  
  // 打印当前配置状态
  Serial.printf("Current LoRa Mode: %d\n", MODE_WOR);
}
