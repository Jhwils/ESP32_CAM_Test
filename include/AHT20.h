#ifndef __TESTTEMP_H__
#define __TESTTEMP_H__

#define AHT20_ADDR 0x38 // I2C地址
#define TRIG_MEAS 0xAC  // 触发测量命令
#define SOFT_RESET 0xBA // 软件复位命令
#define INIT_CMD 0xBE   // 初始化命令
#define BUSY_BIT 0x80   // 忙状态位
#define STATUS_REG 0x71 // 状态寄存器读取地址

bool aht20_init();
void temptest();

#endif
