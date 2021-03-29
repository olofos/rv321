#include <stdint.h>
#include "startup.h"

extern uint32_t _sidata, _sdata, _edata, _sbss, _ebss, _estack;

#ifdef HAVE_INITFINI_ARRAY

/* These magic symbols are provided by the linker.  */
extern void (*__preinit_array_start []) (void) __attribute__((weak));
extern void (*__preinit_array_end []) (void) __attribute__((weak));
extern void (*__init_array_start []) (void) __attribute__((weak));
extern void (*__init_array_end []) (void) __attribute__((weak));

#endif

void __attribute__((naked)) Reset_Handler(void)
{
    uint32_t *src, *dst;

    for(src = &_sidata, dst = &_sdata; dst < &_edata; src++, dst++) {
        *dst = *src;
    }

    for(dst = &_sbss; dst < &_ebss; dst++) {
        *dst = 0;
    }

#ifdef HAVE_INITFINI_ARRAY
    size_t count;
    size_t i;

    count = __preinit_array_end - __preinit_array_start;
    for (i = 0; i < count; i++) {
        __preinit_array_start[i] ();
    }

    count = __init_array_end - __init_array_start;
    for (i = 0; i < count; i++) {
        __init_array_start[i] ();
    }
#endif

    main();

    for(;;) {
    }
}

void __attribute__((naked)) Default_Handler(void)
{
    for(;;) {
    }
}

typedef void (*vector_table_entry_t)(void);

typedef struct {
    uint32_t *initial_sp_value;
    vector_table_entry_t reset; // 004
    vector_table_entry_t nmi;   // 008
    vector_table_entry_t hard_fault; // 00c

    vector_table_entry_t reserved_x0010; // 010
    vector_table_entry_t reserved_x0014; // 014
    vector_table_entry_t reserved_x0018; // 018
    vector_table_entry_t reserved_x001C; // 01c
    vector_table_entry_t reserved_x0020; // 020
    vector_table_entry_t reserved_x0024; // 024
    vector_table_entry_t reserved_x0028; // 028

    vector_table_entry_t sv_call; // 02c
    vector_table_entry_t reserved_x0030; // 030
    vector_table_entry_t reserved_x0034; // 034
    vector_table_entry_t pend_sv; // 038
    vector_table_entry_t systick; // 03c
    vector_table_entry_t wwdg; // 040
    vector_table_entry_t reserved_x0044; // 044
    vector_table_entry_t rtc; // 048
    vector_table_entry_t flash; // 04c
    vector_table_entry_t rcc; // 050
    vector_table_entry_t exti0_1; // 054
    vector_table_entry_t exti2_3; // 058
    vector_table_entry_t exti4_15; // 05c
    vector_table_entry_t reserved_x0060; // 060
    vector_table_entry_t dma1_channel1; // 064
    vector_table_entry_t dma1_channel2_3; // 068
    vector_table_entry_t dma1_channel4_5_dmamux1_ovr; // 06c
    vector_table_entry_t adc1; // 070
    vector_table_entry_t tim1_brk_up_trg_com; // 074
    vector_table_entry_t tim1_cc; // 078
    vector_table_entry_t reserved_x007C; // 07c

    vector_table_entry_t tim3; // 080
    vector_table_entry_t reserved_x0084; // 084
    vector_table_entry_t reserved_x0088; // 088
    vector_table_entry_t tim14; // 08c

    vector_table_entry_t reserved_x0090; // 090
    vector_table_entry_t tim16; // 094
    vector_table_entry_t tim17; // 098
    vector_table_entry_t i2c1; // 09c
    vector_table_entry_t i2c2; // 0a0
    vector_table_entry_t spi1; // 0a4
    vector_table_entry_t spi2; // 0a8
    vector_table_entry_t usart1; // 0ac
    vector_table_entry_t usart2; // 0b0
    vector_table_entry_t reserved_x00B4; // 0b4
} vector_table_t;

#define weak_alias(name, aliasname) _weak_alias (name, aliasname)
#define _weak_alias(name, aliasname) extern __typeof (name) aliasname __attribute__ ((weak, alias (#name)));

weak_alias(Default_Handler, NMI_Handler);
weak_alias(Default_Handler, HardFault_Handler);
weak_alias(Default_Handler, SVC_Handler);
weak_alias(Default_Handler, PendSV_Handler);
weak_alias(Default_Handler, SysTick_Handler);

weak_alias(Default_Handler, WWDG_IRQHandler);
weak_alias(Default_Handler, PVD_IRQHandler);
weak_alias(Default_Handler, RTC_IRQHandler);
weak_alias(Default_Handler, FLASH_IRQHandler);
weak_alias(Default_Handler, RCC_IRQHandler);
weak_alias(Default_Handler, EXTI0_1_IRQHandler);
weak_alias(Default_Handler, EXTI2_3_IRQHandler);
weak_alias(Default_Handler, EXTI4_15_IRQHandler);
weak_alias(Default_Handler, DMA1_Channel1_IRQHandler);
weak_alias(Default_Handler, DMA1_Channel2_3_IRQHandler);
weak_alias(Default_Handler, DMA1_Channel4_5_DMAMUX1_OVR_IRQHandler);
weak_alias(Default_Handler, ADC1_IRQHandler);
weak_alias(Default_Handler, TIM1_BRK_UP_TRG_COM_IRQHandler);
weak_alias(Default_Handler, TIM1_CC_IRQHandler);
weak_alias(Default_Handler, TIM3_IRQHandler);
weak_alias(Default_Handler, TIM14_IRQHandler);
weak_alias(Default_Handler, TIM16_IRQHandler);
weak_alias(Default_Handler, TIM17_IRQHandler);
weak_alias(Default_Handler, I2C1_IRQHandler);
weak_alias(Default_Handler, I2C2_IRQHandler);
weak_alias(Default_Handler, SPI1_IRQHandler);
weak_alias(Default_Handler, SPI2_IRQHandler);
weak_alias(Default_Handler, USART1_IRQHandler);
weak_alias(Default_Handler, USART2_IRQHandler);

vector_table_t vector_table __attribute__ ((section(".isr_vector"))) __attribute__((used)) = {
    .initial_sp_value = &_estack,
    .reset = Reset_Handler,
    .nmi = NMI_Handler,
    .hard_fault = HardFault_Handler,
    .sv_call = SVC_Handler,
    .pend_sv = PendSV_Handler,
    .systick = SysTick_Handler,

    .wwdg = WWDG_IRQHandler,
    .rtc = RTC_IRQHandler,
    .flash = FLASH_IRQHandler,
    .rcc = RCC_IRQHandler,

    .exti0_1 = EXTI0_1_IRQHandler,
    .exti2_3 = EXTI2_3_IRQHandler,
    .exti4_15 = EXTI4_15_IRQHandler,
    .dma1_channel1 = DMA1_Channel1_IRQHandler,
    .dma1_channel2_3 = DMA1_Channel2_3_IRQHandler,
    .dma1_channel4_5_dmamux1_ovr = DMA1_Channel4_5_DMAMUX1_OVR_IRQHandler,
    .adc1 = ADC1_IRQHandler,
    .tim1_brk_up_trg_com = TIM1_BRK_UP_TRG_COM_IRQHandler,
    .tim1_cc = TIM1_CC_IRQHandler,
    .tim3 = TIM3_IRQHandler,
    .tim14 = TIM14_IRQHandler,
    .tim16 = TIM16_IRQHandler,
    .tim17 = TIM17_IRQHandler,
    .i2c1 = I2C1_IRQHandler,
    .i2c2 = I2C2_IRQHandler,
    .spi1 = SPI1_IRQHandler,
    .spi2 = SPI2_IRQHandler,
    .usart1 = USART1_IRQHandler,
    .usart2 = USART2_IRQHandler,

    .reserved_x0010 = (vector_table_entry_t) 0xFFFFFFFF,
    .reserved_x0014 = (vector_table_entry_t) 0xFFFFFFFF,
    .reserved_x0018 = (vector_table_entry_t) 0xFFFFFFFF,
    .reserved_x001C = (vector_table_entry_t) 0xFFFFFFFF,
    .reserved_x0020 = (vector_table_entry_t) 0xFFFFFFFF,
    .reserved_x0024 = (vector_table_entry_t) 0xFFFFFFFF,
    .reserved_x0028 = (vector_table_entry_t) 0xFFFFFFFF,
    .reserved_x0030 = (vector_table_entry_t) 0xFFFFFFFF,
    .reserved_x0034 = (vector_table_entry_t) 0xFFFFFFFF,
    .reserved_x0044 = (vector_table_entry_t) 0xFFFFFFFF,
    .reserved_x0060 = (vector_table_entry_t) 0xFFFFFFFF,
    .reserved_x007C = (vector_table_entry_t) 0xFFFFFFFF,
    .reserved_x0084 = (vector_table_entry_t) 0xFFFFFFFF,
    .reserved_x0088 = (vector_table_entry_t) 0xFFFFFFFF,
    .reserved_x0090 = (vector_table_entry_t) 0xFFFFFFFF,
    .reserved_x00B4 = (vector_table_entry_t) 0xFFFFFFFF,
};
