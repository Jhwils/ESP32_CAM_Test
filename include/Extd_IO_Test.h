#ifndef __EXTD_IO_TEST_H__
#define __EXTD_IO_TEST_H__


//根据芯片原理图，A0=0, A1=0, A2=1;
#define PCA9534_ADDR 0x24
#define REG_INPUT 0x00 // 输入寄存器
#define REG_OUTPUT 0x01 // 输出寄存器   
#define REG_POLARITY 0x02 // 极性寄存器
#define REG_CONFIG 0x03 // 配置寄存器

#define AHT20_ADDR 0x38 // AHT20传感器地址
#define W24C16_Addr 0x50 // W24C16 EEPROM地址

void RTC_SCL_init();
bool PCA_Write(TwoWire* wire, uint8_t reg, uint8_t data);
uint8_t PCA_Read(TwoWire* wire, uint8_t reg);
void printBinary(uint8_t value);



#endif
