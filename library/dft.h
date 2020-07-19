#ifndef DFT_H
#define DFT_H

#include<stdint.h>

#define DFT_SIZE 150

#define SOFT_CNT 8
#define HARD_CNT 4

//! @brief 预处理打表
void dft_init(void);

//! @brief 计算8位谐波分量
void dft_add(uint8_t ch, uint8_t n, uint16_t val);

//! @brief 计算8位谐波分量
uint8_t dft_get(uint8_t ch);

#endif
