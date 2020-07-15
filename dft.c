#include "dft.h"
#include <arm_math.h>

#define N DFT_SIZE
#define K 3

typedef struct {
    float real, imag;
} complex_t;

static complex_t coeff[N];

void dft_init(void) {
    for (uint8_t n = 0; n < N; n++) {
        coeff[n].real = arm_cos_f32(2 * PI / N * K * n);
        coeff[n].imag = arm_sin_f32(2 * PI / N * K * n);
    }
}

uint8_t dft_calculate(uint16_t *ADC_RESULT) {
    complex_t ResultThisNode = {0, 0};
    for (uint8_t n = 0; n < N; ++n) {
        ResultThisNode.real += coeff[n].real * ADC_RESULT[n];
        ResultThisNode.imag += -coeff[n].imag * ADC_RESULT[n];
    }
    float ret;
    arm_sqrt_f32(ResultThisNode.real * ResultThisNode.real +
                     ResultThisNode.imag * ResultThisNode.imag,
                 &ret);
    return ret / 64 / N;  //!< 归一化
}
