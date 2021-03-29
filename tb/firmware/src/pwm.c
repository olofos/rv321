#include "defines.h"

// #define PWMIRQ 1
#define PWMDMA 1

const uint8_t anim_off[] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

const uint8_t anim_red_fast[] = {
    0,  0, 0,  2, 0,  9, 0, 19, 0, 30, 0, 41, 0, 51, 0, 58,
    0, 60, 0, 58, 0, 51, 0, 41, 0, 30, 0, 19, 0,  9, 0,  2
};

const uint8_t anim_yellow_mid[] = {
      0,  0,  1,  0,  4,  1,  8,  2, 15,  3, 22,  4, 31,  6, 40,  8,
     50, 10, 60, 12, 69, 14, 78, 16, 85, 17, 92, 18, 96, 19, 99, 20,
    100, 20, 99, 20, 96, 19, 92, 18, 85, 17, 78, 16, 69, 14, 60, 12,
     50, 10, 40,  8, 31,  6, 22,  4, 15,  3,  8,  2,  4,  1,  1,  0
};

const uint8_t anim_green_slow[] = {0, 0, 0, 0, 2, 0, 4, 0, 7, 0, 10, 0, 15, 0, 20, 0, 25, 0, 31, 0, 37, \
                                   0, 43, 0, 50, 0, 57, 0, 63, 0, 69, 0, 75, 0, 80, 0, 85, 0, 90, 0, 93, \
                                   0, 96, 0, 98, 0, 100, 0, 100, 0, 100, 0, 98, 0, 96, 0, 93, 0, 90, 0, \
                                   85, 0, 80, 0, 75, 0, 69, 0, 63, 0, 57, 0, 50, 0, 43, 0, 37, 0, 31, 0, \
                                   25, 0, 20, 0, 15, 0, 10, 0, 7, 0, 4, 0, 2, 0, 0, 0};

struct anim {
    unsigned len;
    const uint8_t *data;
};

const struct anim anims[] = {
    { sizeof(anim_off), anim_off },
    { sizeof(anim_red_fast), anim_red_fast },
    { sizeof(anim_yellow_mid), anim_yellow_mid },
    { sizeof(anim_green_slow), anim_green_slow },
};

static volatile int current_anim = 0;

void pwm_set_anim(int n)
{
    current_anim = n % (sizeof(anims) / sizeof(anims[0]));
}

void pwm_init(void)
{
#ifdef PWMDMA
    DMA1_Channel1->CCR = 0;

    DMA1_Channel1->CCR =
        DMA_CCR_DIR_MEMORY_TO_PERIPH |
        DMA_CCR_MSIZE_8BITS |
        DMA_CCR_PSIZE_32BITS |
        DMA_CCR_MINC |
        DMA_CCR_TCIE ;

    DMA1_Channel1->CNDTR = anims[current_anim].len;
    DMA1_Channel1->CPAR = (uint32_t) &TIM1->DMAR;
    DMA1_Channel1->CMAR = (uint32_t) anims[current_anim].data;

    DMAMUX1_Channel0->CCR = DMAMUX_CCR_REQ_TIM1_UP;

    DMA1_Channel1->CCR |= DMA_CCR_EN;
#endif

    TIM1->CCMR1 =
        TIM_CCMR1_OC1M_PWM1 | TIM_CCMR1_OC1PE |
        TIM_CCMR1_OC2M_PWM1 | TIM_CCMR1_OC2PE ;

    TIM1->BDTR = TIM_BDTR_MOE;

    TIM1->CCER = TIM_CCER_CC1E | TIM_CCER_CC2E;

    TIM1->PSC = 6400-1;
    TIM1->ARR = 100-1;
    TIM1->RCR = 4-1;

#ifdef PWMIRQ
    TIM1->DIER = TIM_DIER_UIE;
#endif
#ifdef PWMDMA
    TIM1->DIER = TIM_DIER_UDE;
    TIM1->DCR = TIM_DCR_DBL_Val(2) | TIM_DCR_DBA_CCR1;
#endif

    TIM1->CR1 = TIM_CR1_ARPE;

    TIM1->CCR1 = 0;
    TIM1->CCR2 = 0;

#ifdef PWMIRQ
    NVIC_EnableIRQ(TIM1_BRK_UP_TRG_COM_IRQn);
#endif
#ifdef PWMDMA
    NVIC_EnableIRQ(DMA1_Channel1_IRQn);
#endif

    TIM1->CR1 |= TIM_CR1_CEN;
}

#ifdef PWMIRQ
void TIM1_BRK_UP_TRG_COM_IRQHandler(void)
{
    static unsigned n = 0;
    if(TIM1->SR & TIM_SR_UIF) {
        TIM1->CCR1 = anims[current_anim].data[n++];
        TIM1->CCR2 = anims[current_anim].data[n++];
        if(n >= anims[current_anim].len) n = 0;
        TIM1->SR &= ~TIM_SR_UIF;
    }
}
#endif

#ifdef PWMDMA
extern volatile uint8_t test_flag;
void DMA1_Channel1_IRQHandler(void)
{
    if(DMA1->ISR & DMA_ISR_TCIF1) {
        DMA1_Channel1->CCR &= ~DMA_CCR_EN;

        DMA1_Channel1->CNDTR = anims[current_anim].len;
        DMA1_Channel1->CPAR = (uint32_t) &TIM1->DMAR;
        DMA1_Channel1->CMAR = (uint32_t) anims[current_anim].data;

        DMA1_Channel1->CCR |= DMA_CCR_EN;

        DMA1->IFCR = DMA_IFCR_CTCIF1;
    }
}
#endif
