/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2018 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "board.h"
#include "fsl_device_registers.h"
#if defined(SDK_I2C_BASED_COMPONENT_USED) && SDK_I2C_BASED_COMPONENT_USED
#include "fsl_i2c.h"
#endif /* SDK_I2C_BASED_COMPONENT_USED */
#include "fsl_debug_console.h"
#include "peripherals.h"
#include "pin_mux.h"

/*******************************************************************************
 * Variables
 ******************************************************************************/
uint32_t mcgout_clk_mhz = 100, core_clk_mhz = 100, bus_clk_mhz = 100;

/*******************************************************************************
 * Code
 ******************************************************************************/

void Disable_Wdog(void) {
    // Disable the WDOG module
    // WDOG_UNLOCK: WDOGUNLOCK=0xC520
    WDOG->UNLOCK = (uint16_t)0xC520u; // Key 1
    // WDOG_UNLOCK : WDOGUNLOCK=0xD928
    WDOG->UNLOCK = (uint16_t)0xD928u; // Key 2
    // WDOG_STCTRLH:
    // ??=0,DISTESTWDOG=0,BYTESEL=0,TESTSEL=0,TESTWDOG=0,??=0,STNDBYEN=1,WAITEN=1,STOPEN=1,DBGEN=0,ALLOWUPDATE=1,WINEN=0,IRQRSTEN=0,CLKSRC=1,WDOGEN=0
    WDOG->STCTRLH = (uint16_t)0x01D2u;
}

void div_set(void) {
    uint8_t temp_c5, temp_c6;
    uint32_t temp_clkdiv;
    temp_clkdiv = (uint32_t)0x01260000u;
    temp_c5 = MCG_C5_PRDIV0(5 - 1);
    temp_c6 = MCG_C6_PLLS(1) | MCG_C6_VDIV0(40 - 16);
    // temp_c6 = MCG_C6_PLLS(1) | MCG_C6_VDIV0(36 - 16);

    SIM->CLKDIV1 = temp_clkdiv;
    MCG->C5 = temp_c5;
    MCG->C6 = temp_c6;
}

void SYSTEM_CLK(void) {
    uint32_t temp_reg;

    // Switch to FBE Mode
    SIM->CLKDIV1 = (uint32_t)0xffffffffu; // 预分频寄存器，先都设置为1
    OSC->CR = (uint8_t)0x80u;             // 启用外部晶振

    // 外部晶振是有源的时候不要置位MCG_C2_EREFS0_MASK
    // ，这样才能使用A19引脚
    MCG->C2 &= ~MCG_C2_LP_MASK;
    MCG->C2 |= MCG_C2_RANGE0(2);

    // 选择外部时钟作为MCGOUTCLK的输入源，1536分频，FLL输入源为外部时钟，启用MCGIRCLK，内部参考时钟在停止模式下禁止
    MCG->C1 = (uint8_t)0xBAu;
    MCG->C4 &= (uint8_t) ~(uint8_t)0xE0u;
    MCG->C6 &= (uint8_t)(~(uint8_t)MCG_C6_PLLS_MASK); // MCG的输入源选择FLL
    while ((MCG->S & MCG_S_IREFST_MASK) != 0u) {
    } // 等待，直到FLL的时钟源是外部时钟
    while ((MCG->S & MCG_S_CLKST_MASK) != 0x08u) {
    } // 等待，MCGOUTCLK的时钟源被选择为外部时钟

    //保存寄存器值
    temp_reg = FMC->PFAPR;

    //设置M0PFD至M7PFD为1，禁用预取功能
    FMC->PFAPR |= FMC_PFAPR_M7PFD_MASK | FMC_PFAPR_M6PFD_MASK |
                  FMC_PFAPR_M5PFD_MASK | FMC_PFAPR_M4PFD_MASK |
                  FMC_PFAPR_M3PFD_MASK | FMC_PFAPR_M2PFD_MASK |
                  FMC_PFAPR_M1PFD_MASK | FMC_PFAPR_M0PFD_MASK;

    // Switch to PBE Mode
    div_set();
    //恢复寄存器的值
    FMC->PFAPR = temp_reg;

    while ((MCG->S & MCG_S_PLLST_MASK) == 0u) {
    } // Wait until the source of the PLLS clock has switched to the PLL
    while ((MCG->S & MCG_S_LOCK0_MASK) == 0u) {
    } // Wait until locked

    // Switch to PEE Mode
    MCG->C1 = (uint8_t)0x1Au;
    while ((MCG->S & 0x0Cu) != 0x0Cu) {
    } // Wait until output of the PLL is selected
    while ((MCG->S & MCG_S_LOCK0_MASK) == 0u) {
    } // Wait until locked
}

void get_clk(void) {
    mcgout_clk_mhz = EX_REF_CLK * ((MCG->C6 & MCG_C6_VDIV0_MASK) + 16) /
                     ((MCG->C5 & MCG_C5_PRDIV0_MASK) + 1) / 2;
    core_clk_mhz =
        mcgout_clk_mhz / ((SIM->CLKDIV1 >> SIM_CLKDIV1_OUTDIV1_SHIFT) + 1);
    bus_clk_mhz = mcgout_clk_mhz /
                  (((SIM->CLKDIV1 >> SIM_CLKDIV1_OUTDIV2_SHIFT) & 0x0f) + 1);
}

void board_init(void) {
    // set CP10, CP11 Full Access
    SCB->CPACR |= ((3UL << 10 * 2) | (3UL << 11 * 2));
    Disable_Wdog();
    SYSTEM_CLK();
    get_clk();
    BOARD_InitPins();
    BOARD_InitPeripherals();

    UART_Init(UART3_PERIPHERAL, &UART3_config, bus_clk_mhz * 1000000);
    // DbgConsole_Init(BOARD_DEBUG_UART_INSTANCE, BOARD_DEBUG_UART_BAUDRATE,
    // BOARD_DEBUG_UART_TYPE, BOARD_DEBUG_UART_CLK_FREQ);
}

void NMI_Handler(void) {}
