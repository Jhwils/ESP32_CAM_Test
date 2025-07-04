#ifndef __EXTD_IO_TEST_H__
#define __EXTD_IO_TEST_H__

void RTC_SCL_init();
void PCA_Write(uint8_t reg, uint8_t data);  
uint8_t PCA_Read(uint8_t reg);
void printBinary(uint8_t value);



#endif
