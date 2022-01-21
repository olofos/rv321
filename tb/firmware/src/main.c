#include "defines.h"
#include "systick.h"
#include "uart.h"

#include "spi.h"
#include "mcp2317.h"
#include "cbuf.h"
#include "pwm.h"

#include "console.h"

#include <unistd.h>
#include <stdio.h>
#include <string.h>

#define CS_GPIO GPIOA
#define CS (1UL << 4)

#define LEDG_GPIO GPIOA
#define LEDG (1UL << 8)

#define LEDR_GPIO GPIOA
#define LEDR (1UL << 9)

#define INT_GPIO GPIOB
#define INT (1UL << 3)

int main(void)
{
    RCC->CR = RCC_CR_HSION;

    RCC->IOPENR = RCC_IOPENR_GPIOAEN | RCC_IOPENR_GPIOBEN;
    RCC->AHBENR = RCC_AHBENR_FLASHEN | RCC_AHBENR_DMA1EN;
    RCC->APBENR1 = RCC_APBENR1_USART2EN;
    RCC->APBENR2 = RCC_APBENR2_SYSCFGEN | RCC_APBENR2_TIM1EN | RCC_APBENR2_SPI1EN;

    FLASH->ACR |= FLASH_ACR_PRFTEN;

    do {
        FLASH->ACR |= FLASH_ACR_LATENCY_2WS;
    } while((FLASH->ACR & FLASH_ACR_LATENCY_Msk) != 2UL);

    while(RCC->CR & RCC_CR_PLLRDY) {}

    RCC->PLLCFGR = RCC_PLLCFGR_PLLR_Val(2) | RCC_PLLCFGR_PLLN_Val(8) | RCC_PLLCFGR_PLLM_Val(1) | RCC_PLLCFGR_PLLSRC_HSI;

    RCC->CR |= RCC_CR_PLLON;

    while(!(RCC->CR & RCC_CR_PLLRDY)) {}

    RCC->PLLCFGR |= RCC_PLLCFGR_PLLREN;

    RCC->CFGR |= RCC_CFGR_SW_PLLRCLK;

    systick_init();

    SYSCFG->CFGR1 = SYSCFG_CFGR1_PA11_RMP;

    GPIOA->AFR[0] =
        GPIOA_AFR0_PA2_UART2_TX  |
        GPIOA_AFR0_PA3_UART2_RX  |
        GPIOA_AFR0_PA5_SPI1_SCK  |
        GPIOA_AFR0_PA6_SPI1_MISO |
        GPIOA_AFR0_PA7_SPI1_MOSI ;

    GPIOA->AFR[1] =
        GPIOA_AFR1_PA8_TIM1_CH1 |
        GPIOA_AFR1_PA9_TIM1_CH2 |
        GPIOA_AFR1_PA13_SWDIO   |
        GPIOA_AFR1_PA14_SWCLK   ;

    GPIOA->OTYPER = 0;
    GPIOA->OSPEEDR =
        (GPIO_OSPEEDR_MID << 2)  | // PA2: UART2_TX
        (GPIO_OSPEEDR_MID << 4)  | // PA4: CS
        (GPIO_OSPEEDR_HI  << 5)  | // PA5: SPI1_SCK
        (GPIO_OSPEEDR_HI  << 7)  | // PA7: SPI1_MOSI
        (GPIO_OSPEEDR_MID << 8)  | // PA8: TIM1_CH1
        (GPIO_OSPEEDR_MID << 9)  | // PA9: TIM1_CH2
        (GPIO_OSPEEDR_VHI << 13) ; // PA13: SWDIO

    GPIOA->PUPDR =
        (GPIO_PUPDR_PU << 26) | // PA13: SWDIO
        (GPIO_PUPDR_PD << 28) ; // PA14: SWCLK

    GPIOA->MODER =
        (GPIO_MODER_ANALOG << 0)  | // PA0: Unused
        (GPIO_MODER_ANALOG << 2)  | // PA1: Unused
        (GPIO_MODER_AF     << 4)  | // PA2: UART2_TX
        (GPIO_MODER_AF     << 6)  | // PA3: UART2_RX
        (GPIO_MODER_OUTPUT << 8)  | // PA4: CS
        (GPIO_MODER_AF     << 10) | // PA5: SPI1_SCK
        (GPIO_MODER_AF     << 12) | // PA6: SPI1_MISO
        (GPIO_MODER_AF     << 14) | // PA7: SPI1_MOSI
        (GPIO_MODER_AF     << 16) | // PA8: TIM1_CH1
        (GPIO_MODER_AF     << 18) | // PA9: TIM1_CH2
        // (GPIO_MODER_OUTPUT    << 18) | // PA9: TIM1_CH2
        (GPIO_MODER_ANALOG << 20) | // PA10: Unused
        (GPIO_MODER_ANALOG << 22) | // PA11: Unused
        (GPIO_MODER_ANALOG << 24) | // PA12: Unused
        (GPIO_MODER_AF     << 26) | // PA13: SWDIO
        (GPIO_MODER_AF     << 28) | // PA14: SWCLK
        (GPIO_MODER_ANALOG << 30) ; // PA15: Unused

    GPIOB->MODER =
        (GPIO_MODER_ANALOG << 0)  |
        (GPIO_MODER_ANALOG << 2)  |
        (GPIO_MODER_ANALOG << 4)  |
        (GPIO_MODER_INPUT  << 6)  | // PB3: INT
        (GPIO_MODER_ANALOG << 8)  |
        (GPIO_MODER_ANALOG << 10) |
        (GPIO_MODER_ANALOG << 12) |
        (GPIO_MODER_ANALOG << 14) |
        (GPIO_MODER_ANALOG << 16) |
        (GPIO_MODER_ANALOG << 18) |
        (GPIO_MODER_ANALOG << 20) |
        (GPIO_MODER_ANALOG << 22) |
        (GPIO_MODER_ANALOG << 24) |
        (GPIO_MODER_ANALOG << 26) |
        (GPIO_MODER_ANALOG << 28) |
        (GPIO_MODER_ANALOG << 30) ;


    GPIOA->BSRR = (LEDR << 16) | (LEDG << 16) | CS;

    pwm_init();

    uart_init();

    // for(;;) {
    //     const char *s = "0123456789abcdef";
    //     write(1, s, strlen(s));
    //     delay(250);
    // }

    // puts("\e[2J\r\nStarting...\r\n");

    delay(100);

    spi_init();
    mcp2317_init();

    // for(int i = 0; i < 5; i++) {
    //     mcp2317_write(i, MCP2317_REG_IOCON, MCP2317_MIRROR | MCP2317_HAEN | MCP2317_ODR);
    // }

    // printf("IOCON:\r\n");
    // for(int i = 0; i < 5; i++) {
    //     uint8_t d = mcp2317_read(i, MCP2317_REG_IOCON);
    //     printf("%d: %02X\r\n", i, d);
    // }

    // printf("IODIRA:\r\n");
    // for(int i = 0; i < 5; i++) {
    //     uint8_t d = mcp2317_read(i, MCP2317_REG_IODIRA);
    //     printf("%d: %02X\r\n", i, d);
    // }

    // printf("IOPOLA:\r\n");
    // for(int i = 0; i < 5; i++) {
    //     uint8_t d = mcp2317_read(i, MCP2317_REG_IPOLA);
    //     printf("%d: %02X\r\n", i, d);
    // }

    // printf("IOCON:\r\n");
    // for(int i = 0; i < 5; i++) {
    //     uint8_t d = mcp2317_read(i, MCP2317_REG_IOCON);
    //     printf("%d: %02X\r\n", i, d);
    // }

    // for(int j = 0; j < 5; j++) {
    //     mcp2317_write(j, MCP2317_REG_IPOLA, 1 << j);
    //     printf("IOPOLA:\r\n");
    //     for(int i = 0; i < 5; i++) {
    //         uint8_t d = mcp2317_read(i, MCP2317_REG_IPOLA);
    //         printf("%d: %02X\r\n", i, d);
    //     }

    // }

    // for(int j = 0; j < 5; j++) {
    //     mcp2317_write(j, MCP2317_REG_IPOLA, 0);
    // }

    // printf("IOPOLA:\r\n");
    // for(int i = 0; i < 5; i++) {
    //     uint8_t d = mcp2317_read(i, MCP2317_REG_IPOLA);
    //     printf("%d: %02X\r\n", i, d);
    // }

    // // mcp2317_write(1, MCP2317_REG_GPPUA, 0x55);

    // printf("GPIO:\r\n");
    // for(int i = 0; i < 5; i++) {
    //     uint8_t d1 = mcp2317_read(i, MCP2317_REG_GPIOA);
    //     uint8_t d2 = mcp2317_read(i, MCP2317_REG_GPIOB);
    //     printf("%d: %02X %02X\r\n", i, d1, d2);
    // }

    // mcp2317_write(0, MCP2317_REG_IODIRB, (uint8_t)~0xC0);
    // mcp2317_write(1, MCP2317_REG_IODIRA, (uint8_t)~0xFF);
    // mcp2317_write(1, MCP2317_REG_IODIRB, (uint8_t)~0xFF);

    // for(uint32_t j = 0 ;; j++) {
    //     printf("%04lx\r\n", j);
    //     delay(500);

    //     mcp2317_write(0, MCP2317_REG_OLATB, 0x80);
    //     mcp2317_write(1, MCP2317_REG_OLATA, 0x55);
    //     mcp2317_write(1, MCP2317_REG_OLATB, 0x55);
    //     delay(500);
    //     mcp2317_write(0, MCP2317_REG_OLATB, 0x40);
    //     mcp2317_write(1, MCP2317_REG_OLATA, ~0x55);
    //     mcp2317_write(1, MCP2317_REG_OLATB, ~0x55);
    // }

    console_init();


    for(;;) {
        // if((systick & 0x3FF) == 0) {
        //     mcp2317_write(0, MCP2317_REG_OLATB, 0x80);
        //     mcp2317_write(1, MCP2317_REG_OLATA, 0x55);
        //     mcp2317_write(1, MCP2317_REG_OLATB, 0x55);
        // } else if((systick & 0x3FF) == 0x200) {
        //     mcp2317_write(0, MCP2317_REG_OLATB, 0x40);
        //     mcp2317_write(1, MCP2317_REG_OLATA, ~0x55);
        //     mcp2317_write(1, MCP2317_REG_OLATB, ~0x55);
        // }
        console_process();
    }
}
