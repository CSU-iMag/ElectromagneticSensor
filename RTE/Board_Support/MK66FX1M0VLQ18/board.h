/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2018 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _BOARD_H_
#define _BOARD_H_

#include <MK66F18.h>
#include <stdint.h>

#define BOARD_DEBUG_UART_TYPE kSerialPort_Uart
#define BOARD_DEBUG_UART_BASEADDR (uint32_t) UART3
#define BOARD_DEBUG_UART_INSTANCE 0U
#define BOARD_DEBUG_UART_CLKSRC SYS_CLK
#define BOARD_DEBUG_UART_CLK_FREQ core_clk_mhz
#define BOARD_UART_IRQ UART3_RX_TX_IRQn
#define BOARD_UART_IRQ_HANDLER UART3_RX_TX_IRQHandler
#define BOARD_DEBUG_UART_BAUDRATE 115200

/*! @brief The CMP instance/channel used for board. */
#define BOARD_CMP_BASEADDR CMP2
#define BOARD_CMP_CHANNEL 2U

/* DAC base address */
#define BOARD_DAC_BASEADDR DAC0

/* Board accelerometer driver */
#define BOARD_ACCEL_FXOS


#define EX_REF_CLK  50 //(定义外部参考时钟为50MHZ)
extern uint32_t mcgout_clk_mhz, core_clk_mhz, bus_clk_mhz;

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/*******************************************************************************
 * API
 ******************************************************************************/
void board_init(void);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* _BOARD_H_ */
