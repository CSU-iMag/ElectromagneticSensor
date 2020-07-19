#include "dft.h"
#include "MK60_adc.h"
#include "MK60_pit.h"
#include <stdbool.h>

__PACKED_STRUCT {
    uint8_t SOH;
    uint8_t soft[SOFT_CNT];
    uint16_t hard[HARD_CNT];
    uint8_t parity;
    uint8_t EOT;
}
static Buffer;
 
static const ADCn_Ch HARD_CH[HARD_CNT] = { ADC0_SE15, ADC0_SE13, ADC0_SE16, ADC1_SE16}, 
                     SOFT_CH[SOFT_CNT] = { ADC0_SE8, ADC0_SE9, ADC0_SE10, ADC0_SE11, 
                                           ADC0_SE12, ADC0_SE14, ADC0_SE17, ADC0_SE18 };
                     
static uint32_t HardSum[HARD_CNT];
static uint16_t SampleData[SOFT_CNT][DFT_SIZE];
static volatile bool flag;

__STATIC_INLINE void Accumulate(void) {
    uint8_t sum = 0;
    for (int i = 0; i < HARD_CNT*2 + SOFT_CNT; ++i)
        sum += *((uint8_t *)(&Buffer) + 1 + i);
    Buffer.parity = sum;
}

//static void LoopComplete(void) {
//    uint32_t pri = DisableGlobalIRQ();
//    MajorLoop = 0;
//    for (int i = 0; i < SOFT_CNT; ++i)
//        Buffer.soft[i] = dft_calculate(SampleData[i]);
//    for (int i = 0; i < 4; ++i) {
//        ADC16_SetChannelConfig(ADC0_PERIPHERAL, 0U,
//                               &ADC0_channelsConfig[8 + i]);
//        while ((ADC0->SC1[0] & ADC_SC1_COCO_MASK) != ADC_SC1_COCO_MASK)
//            ;
//        Buffer.hard[i] = ADC0->R[0];
//        ADC0->SC1[0] &= ~ADC_SC1_COCO_MASK;
//    }
//    Accumulate();
//    UART_WriteBlocking(UART3, (const uint8_t *)&Buffer, sizeof Buffer);
//    EnableGlobalIRQ(pri);
//}

int main(void) {
    int i, j;
    volatile uint64_t cnt;
    
    get_clk();
    dft_init();
    Buffer.SOH = 0x01;
    Buffer.EOT = 0x04;

    for (i = 0; i < SOFT_CNT; ++i)
        adc_init(SOFT_CH[i]);
    for (i = 0; i < HARD_CNT; ++i)
        adc_init(HARD_CH[i]);
    
    uart_init(uart3, 1000000);
    pit_init_us(pit0, 51);
    
    while (1) { 
        for (i = 0; i < DFT_SIZE; ++i) {
            while (!flag) 
                ++cnt;
            flag = false;
            
            for (j = 0; j < SOFT_CNT; ++j) 
                SampleData[j][i] = adc_once(SOFT_CH[j], ADC_16bit);
            for (j = 0; j < SOFT_CNT; ++j)
                dft_add(j, i, SampleData[j][i]);
            
            if (i < DFT_SIZE - 1)
                for (j = 0; j < HARD_CNT; ++j) 
                    HardSum[j] += adc_once(HARD_CH[j], ADC_16bit);
        } 
        cnt = 0;
        
        for (i = 0; i < SOFT_CNT; ++i)
            Buffer.soft[i] = dft_get(i);
        for (i = 0; i < HARD_CNT; HardSum[i++] = 0) 
            Buffer.hard[i] = HardSum[i] / (DFT_SIZE - 1);
        Accumulate();
        uart_putbuff(uart3, (uint8_t*)&Buffer, 19);
    }
}

void PIT0_IRQHandler(void) {
	PIT_FlAG_CLR(pit0);
    flag = true;
    __DSB();
}

//static void SampleSoft(void) {
//    SampleData[MinorLoop++][MajorLoop] =
//        ADC16_GetChannelConversionValue(ADC0_PERIPHERAL, 0U);
//    if (MinorLoop < SOFT_CNT)
//        ADC16_SetChannelConfig(ADC0_PERIPHERAL, 0U,
//                               &ADC0_channelsConfig[MinorLoop]);
//}

///* PIT0_IRQn interrupt handler */
//void PIT_CHANNEL_0_IRQHANDLER(void) {
//    PIT_ClearStatusFlags(PIT, kPIT_Chnl_0, kPIT_TimerFlag);
//    ++MajorLoop;
//    if (MajorLoop >= DFT_SIZE)
//        flag = true;
//    MinorLoop = 0;
//    ADC16_SetChannelConfig(ADC0_PERIPHERAL, 0U, &ADC0_channelsConfig[0]);

//    
//}

///* PIT1_IRQn interrupt handler */
//void PIT_CHANNEL_1_IRQHANDLER(void) {
//    PIT_ClearStatusFlags(PIT, kPIT_Chnl_1, kPIT_TimerFlag);
//    cpu_usage = 0;
//    __DSB();
//}

///* ADC0_IRQn interrupt handler */
//void ADC0_IRQHANDLER(void) {
//    SampleSoft();
//    /* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F
//       Store immediate overlapping exception return operation might vector to
//       incorrect interrupt. */
//    __DSB();
//}
