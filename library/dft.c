#include "dft.h"
#include <arm_math.h>

#define N DFT_SIZE
#define K 3

typedef struct {
    float real, imag;
} complex_t;

static complex_t coeff[N], result[SOFT_CNT];

void dft_init(void) {
    for (uint8_t n = 0; n < N; n++) {
        coeff[n].real = arm_cos_f32(2 * PI / N * K * n);
        coeff[n].imag = arm_sin_f32(2 * PI / N * K * n);
    }
}

void dft_add(uint8_t ch, uint8_t n, uint16_t val) {
    result[ch].real += coeff[n].real * val;
    result[ch].imag += -coeff[n].imag * val;
}

uint8_t dft_get(uint8_t ch) {
    float ret;
    arm_sqrt_f32(result[ch].real * result[ch].real +
                     result[ch].imag * result[ch].imag,
                 &ret);
    memset(&result[ch], 0, sizeof(result[ch]));
    
    ret /= N * 80;
    if (ret > 255)
        ret = 255;
    return ret;  
}
