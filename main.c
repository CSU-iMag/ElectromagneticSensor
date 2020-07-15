#include "board.h"
#include "dft.h"
#include "peripherals.h"

#define SOFT_CNT 8
#define HARD_CNT 4

__PACKED_STRUCT {
    uint8_t SOH;
    uint8_t soft[SOFT_CNT];
    uint16_t hard[HARD_CNT];
    uint8_t parity;
    uint8_t EOT;
}
static Buffer;

uint16_t SampleData[8][DFT_SIZE];
static uint8_t MajorLoop, MinorLoop;
static volatile bool flag;

volatile static uint32_t cpu_usage;

__STATIC_INLINE void Accumulate() {
    uint8_t sum = 0;
    for (int i = 0; i < HARD_CNT*2 + SOFT_CNT; ++i)
        sum += *((uint8_t *)(&Buffer) + 1 + i);
    Buffer.parity = sum;
}

static void LoopComplete(void) {
    uint32_t pri = DisableGlobalIRQ();
    MajorLoop = 0;
    for (int i = 0; i < SOFT_CNT; ++i)
        Buffer.soft[i] = dft_calculate(SampleData[i]);
    for (int i = 0; i < 4; ++i) {
        ADC16_SetChannelConfig(ADC0_PERIPHERAL, 0U,
                               &ADC0_channelsConfig[8 + i]);
        while ((ADC0->SC1[0] & ADC_SC1_COCO_MASK) != ADC_SC1_COCO_MASK)
            ;
        Buffer.hard[i] = ADC0->R[0];
        ADC0->SC1[0] &= ~ADC_SC1_COCO_MASK;
    }
    Accumulate();
    UART_WriteBlocking(UART3, (const uint8_t *)&Buffer, sizeof Buffer);
    EnableGlobalIRQ(pri);
}

int main(void) {
    DisableGlobalIRQ();
    board_init();
    dft_init();
    Buffer.SOH = 0x01;
    Buffer.EOT = 0x04;
    EnableGlobalIRQ(0);

    while (1) {
        if (flag) {
            LoopComplete();
            flag = false;
        }
//        ++cpu_usage;
    }
}

static void SampleSoft(void) {
    SampleData[MinorLoop++][MajorLoop] =
        ADC16_GetChannelConversionValue(ADC0_PERIPHERAL, 0U);
    if (MinorLoop < SOFT_CNT)
        ADC16_SetChannelConfig(ADC0_PERIPHERAL, 0U,
                               &ADC0_channelsConfig[MinorLoop]);
}

/* PIT0_IRQn interrupt handler */
void PIT_CHANNEL_0_IRQHANDLER(void) {
    PIT_ClearStatusFlags(PIT, kPIT_Chnl_0, kPIT_TimerFlag);
    ++MajorLoop;
    if (MajorLoop >= DFT_SIZE)
        flag = true;
    MinorLoop = 0;
    ADC16_SetChannelConfig(ADC0_PERIPHERAL, 0U, &ADC0_channelsConfig[0]);

    __DSB();
}

/* PIT1_IRQn interrupt handler */
void PIT_CHANNEL_1_IRQHANDLER(void) {
    PIT_ClearStatusFlags(PIT, kPIT_Chnl_1, kPIT_TimerFlag);
    cpu_usage = 0;
    __DSB();
}

/* ADC0_IRQn interrupt handler */
void ADC0_IRQHANDLER(void) {
    SampleSoft();
    /* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F
       Store immediate overlapping exception return operation might vector to
       incorrect interrupt. */
    __DSB();
}
