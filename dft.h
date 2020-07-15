#ifndef DFT_H
#define DFT_H

#include<stdint.h>

#define DFT_SIZE 150

//! @brief 预处理打表
void dft_init(void);

//! @brief 计算8位谐波分量
uint8_t dft_calculate(uint16_t *ADC_RESULT);

#endif
